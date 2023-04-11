#include <Renderer.hpp>
#include <Scene.hpp>

void Renderer::Initialize()
{
    glLineWidth(3.0f); $gl_chk
    glEnable(GL_DEPTH_TEST); $gl_chk

    hairProg.CreatePipelineFromFiles("shaders/simple.vert", "shaders/simple.frag");
    hairProg.Use();
    hairProg.SetClearColor({0.0f, 0.0f, 0.0f, 0.0f});

    surfaceProg.CreatePipelineFromFiles("shaders/surface.vert", "shaders/surface.frag");
    surfaceProg.Use();
    surfaceProg.SetClearColor({0.0f, 0.0f, 0.0f, 0.0f});

    scene->init(*this);
}

void Renderer::Render()
{
    this->frameCount += 1;
    surfaceProg.Clear();
    hairProg.Clear();

    RenderHairs();
    RenderSurfaces();
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

    hairProg.SetUniform("uTModel", glm::mat4(1.0f));
    hairProg.SetUniform("uTView", scene->cam.view());
    hairProg.SetUniform("uTProj", scene->cam.proj({windowSize}));

    scene->hairMesh.draw(hairProg); //todo index this into an array and loop over it
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

    surfaceProg.SetUniform("light_pos", scene->light.pos);
    surfaceProg.SetUniform("light_color", scene->light.color);
    surfaceProg.SetUniform("light_intensity", scene->light.intensity);
    surfaceProg.SetUniform("ambient", scene->surfaceMesh.material.ambient);
    surfaceProg.SetUniform("diffuse", scene->surfaceMesh.material.diffuse);
    surfaceProg.SetUniform("specular", scene->surfaceMesh.material.specular);
    surfaceProg.SetUniform("shininess", scene->surfaceMesh.material.shininess);

    scene->surfaceMesh.draw(surfaceProg); //todo index this into an array and loop over it
}