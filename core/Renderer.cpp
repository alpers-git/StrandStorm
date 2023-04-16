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
    hairProg.Use();
    hairProg.SetClearColor({0.8f, 0.8f, 0.8f, 0.0f});

    surfaceProg.CreatePipelineFromFiles(
        "shaders/surface.vert",
        "shaders/surface.frag");
    surfaceProg.Use();
    surfaceProg.SetClearColor({0.8f, 0.8f, 0.8f, 0.0f});


    shadowProg.CreatePipelineFromFiles(
        "shaders/shadow.vert",
        "shaders/shadow.frag");
    shadowProg.SetClearColor({0.8f, 0.8f, 0.8f, 0.0f});

    opacityShadowProg.CreatePipelineFromFiles(
        "shaders/opacity_sh.vert",
        "shaders/opacity_sh.frag");
    opacityShadowProg.SetClearColor({0.8f, 0.8f, 0.8f, 0.0f});

    scene->init(*this);

    // const GLuint N = HairMesh::controlHairLen;
    // const GLuint M = HairMesh::subdivide;
    // const GLuint H = scene->hairMesh.numHairs();
    // csHair.compile("shaders/hair_gen.comp");
    // csHair.assocBuffer(0, scene->hairMesh.vbo); // InPos
    // csHair.createBuffer(1, (3*N - 2) * H * sizeof(glm::vec4)); // ControlPoints
    // csHair.createBuffer(2, (N-1) * M * H * sizeof(glm::vec4)); // OutPos
    // csHair.setUniform("N", N);
    // csHair.setUniform("M", M);
    // csHair.bindBuffers();
    // csHair.run({H, 1, 1});
}

void Renderer::RenderFirstPass()
{
    //render shadow map
    shadowProg.Use();
    shadowProg.SetUniform("to_clip_space", scene->light.CalculateLightSpaceMatrix());// mvp
    scene->light.shadowTexture->Render([&]() {
            scene->surfaceMesh.draw(shadowProg);
            scene->hairMesh.draw(shadowProg);//to be removed when opacity shadowmaps are done
        });

    //render depthTexture for opacity shadowmap
    auto& depthTex =  scene->light.opacityShadowMaps.depthTex;
    shadowProg.Use();
    shadowProg.SetUniform("to_clip_space", scene->light.CalculateLightSpaceMatrix());// we dont have to set mvp again
    depthTex->Render([&]() {
            scene->hairMesh.draw(shadowProg);
        });
    //render opacitymaps for opacity shadowmap
    opacityShadowProg.Use();
    opacityShadowProg.SetUniform("to_clip_space", scene->light.CalculateLightSpaceMatrix());// we dont have to set mvp again
    opacityShadowProg.SetUniform("depth_map", (int)depthTex->texUnit - GL_TEXTURE0);
    opacityShadowProg.SetUniform("screen_res", depthTex->dims);
    opacityShadowProg.SetUniform("dk", scene->light.opacityShadowMaps.dk);
    scene->light.opacityShadowMaps.opacitiesTex->Render([&]() {
            depthTex->Bind();
            scene->hairMesh.draw(opacityShadowProg);
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

    shadowProg.Clear();
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
    glEnable(GL_BLEND); $gl_chk

    hairProg.SetUniform("uTModel", glm::mat4(1.0f));
    hairProg.SetUniform("uTView", scene->cam.view());
    hairProg.SetUniform("uTProj", scene->cam.proj({windowSize}));

    scene->hairMesh.draw(hairProg); //todo index this into an array and loop over it
    glDisable(GL_BLEND); $gl_chk
}


void Renderer::RenderSurfaces()
{
    surfaceProg.Use();

    glm::mat4 to_clip_space = scene->cam.proj({windowSize}) * scene->cam.view();
    glm::mat4 to_view_space = scene->cam.view();
    glm::mat3 normals_to_view_space = glm::mat3(glm::transpose(glm::inverse(to_view_space)));
    surfaceProg.SetUniform("to_clip_space", to_clip_space);// mvp
    surfaceProg.SetUniform("to_view_space", to_view_space);//mv
    surfaceProg.SetUniform("normals_to_view_space", normals_to_view_space);//mv for normals
    surfaceProg.SetUniform("to_world_space", glm::mat4(1.0f));//m For future use

    const glm::mat4 shadowMatrix = glm::mat4(
						0.5, 0.0, 0.0, 0.0,
						0.0, 0.5, 0.0, 0.0,
						0.0, 0.0, 0.5, 0.0,
						0.5, 0.5, 0.498, 1.0
					) * scene->light.CalculateLightSpaceMatrix();

    surfaceProg.SetUniform("to_light_view_space", shadowMatrix);

    surfaceProg.SetUniform("shadow_map", (int)scene->light.shadowTexture->texUnit - GL_TEXTURE0);
    scene->light.shadowTexture->Bind();

    surfaceProg.SetUniform("light_dir", scene->light.dir);
    surfaceProg.SetUniform("light_color", scene->light.color);
    surfaceProg.SetUniform("light_intensity", scene->light.intensity);
    surfaceProg.SetUniform("ambient", scene->surfaceMesh.material.ambient);
    surfaceProg.SetUniform("diffuse", scene->surfaceMesh.material.diffuse);
    surfaceProg.SetUniform("specular", scene->surfaceMesh.material.specular);
    surfaceProg.SetUniform("shininess", scene->surfaceMesh.material.shininess);

    scene->surfaceMesh.draw(surfaceProg); //todo index this into an array and loop over it
}