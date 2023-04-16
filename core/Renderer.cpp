#include <Renderer.hpp>
#include <Scene.hpp>

void Renderer::Initialize()
{
    glLineWidth(5.0f); $gl_chk
    glEnable(GL_DEPTH_TEST); $gl_chk
    
    // enable alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); $gl_chk

    hairProg.CreatePipelineFromFiles(
        "shaders/hair.vert",
        "shaders/hair.frag");
    hairProg.SetClearColor({0.8f, 0.8f, 0.8f, 0.0f});

    surfaceProg.CreatePipelineFromFiles(
        "shaders/surface.vert",
        "shaders/surface.frag");
    surfaceProg.SetClearColor({0.8f, 0.8f, 0.8f, 0.0f});

    hairShadowProg.CreatePipelineFromFiles(
        "shaders/hair_shadow.vert",
        "shaders/hair_shadow.frag");
    hairShadowProg.SetClearColor({0.8f, 0.8f, 0.8f, 0.0f});

    scene->init(*this);

    // Initialize hair generation / interpolation compute shader
    csHair.compile("shaders/hair_gen.comp");
    scene->hairMesh.bindToComputeShader(csHair);
}

void Renderer::RenderFirstPass()
{

    //render hair shadow map
    hairShadowProg.Use();
    hairShadowProg.SetUniform("to_screen_space", scene->light.CalculateLightSpaceMatrix());// mvp
    scene->light.hairShadowTexture->Render([&]() {
        scene->hairMesh.draw(hairShadowProg);
    });
} 

void Renderer::RenderMainPass()
{
    RenderSurfaces();
    RenderHairs();
}

void Renderer::Render()
{
    this->frameCount += 1;
    
    // Run hair generation compute shader
    csHair.bindBuffers();
    csHair.run({scene->hairMesh.numHairs(), 1, 1});

    hairShadowProg.Clear();
    RenderFirstPass();
    hairProg.Clear();
    RenderMainPass();
}

void Renderer::OnWindowResize(int width, int height)
{
    this->windowSize = {width, height};
    glViewport(0, 0, width, height); $gl_chk
}

void Renderer::OnMouseMove(double x, double y)
{
    mouseInteraction.delta = {x - mouseInteraction.lastPos.x, y - mouseInteraction.lastPos.y};
    mouseInteraction.lastPos = {x, y};

    if(mouseInteraction.leftButton)
        scene->cam.orient(mouseInteraction.delta);// Im not correct fix me
    if(mouseInteraction.rightButton)
        scene->cam.zoom(mouseInteraction.delta.y * 0.01f);
    if(mouseInteraction.middleButton)
        scene->cam.strafe(mouseInteraction.delta * 0.01f);
}

void Renderer::OnMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
        mouseInteraction.leftButton = true;
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        mouseInteraction.leftButton = false;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        mouseInteraction.rightButton = true;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        mouseInteraction.rightButton = false;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        mouseInteraction.middleButton = true;
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
        mouseInteraction.middleButton = false;
}

void Renderer::RenderHairs()
{

    hairProg.Use();
    glEnable(GL_BLEND) $gl_chk;

    hairProg.SetUniform("uTModel", glm::mat4(1.0f));
    hairProg.SetUniform("uTView", scene->cam.view());
    hairProg.SetUniform("uTProj", scene->cam.proj({windowSize}));

    scene->hairMesh.draw(hairProg);

    glDisable(GL_BLEND) $gl_chk;
}


void Renderer::RenderSurfaces()
{
    surfaceProg.Use();

    glm::mat4 to_screen_space = scene->cam.proj({windowSize}) * scene->cam.view();
    glm::mat4 to_view_space = scene->cam.view();
    glm::mat3 normals_to_view_space = glm::mat3(glm::transpose(glm::inverse(to_view_space)));
    surfaceProg.SetUniform("to_screen_space", to_screen_space);// mvp
    surfaceProg.SetUniform("to_view_space", to_view_space);//mv
    surfaceProg.SetUniform("normals_to_view_space", normals_to_view_space);//mv for normals
    surfaceProg.SetUniform("to_world_space", glm::mat4(1.0f));//m For future use

    const glm::mat4 shadowMatrix = glm::mat4(
						0.5, 0.0, 0.0, 0.0,
						0.0, 0.5, 0.0, 0.0,
						0.0, 0.0, 0.5, 0.0,
						0.5, 0.5, 0.49999, 1.0
					) * scene->light.CalculateLightSpaceMatrix();

    surfaceProg.SetUniform("to_light_view_space", shadowMatrix);

    surfaceProg.SetUniform("shadow_map", (int)scene->light.hairShadowTexture->texUnit - GL_TEXTURE0);
    scene->light.hairShadowTexture->Bind();

    surfaceProg.SetUniform("light_dir", scene->light.dir);
    surfaceProg.SetUniform("light_color", scene->light.color);
    surfaceProg.SetUniform("light_intensity", scene->light.intensity);
    surfaceProg.SetUniform("ambient", scene->surfaceMesh.material.ambient);
    surfaceProg.SetUniform("diffuse", scene->surfaceMesh.material.diffuse);
    surfaceProg.SetUniform("specular", scene->surfaceMesh.material.specular);
    surfaceProg.SetUniform("shininess", scene->surfaceMesh.material.shininess);

    scene->surfaceMesh.draw(surfaceProg); //todo index this into an array and loop over it
}