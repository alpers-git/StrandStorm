#include <Renderer.hpp>
#include <Scene.hpp>

void Renderer::Initialize()
{
    glLineWidth(3.0f); $gl_chk
    glEnable(GL_DEPTH_TEST); $gl_chk

    prog.CreatePipelineFromFiles("shaders/simple.vert", "shaders/simple.frag");
    prog.Use();
    prog.SetClearColor({0.0f, 0.0f, 0.0f, 0.0f});

    scene->init(*this);
}

void Renderer::Render()
{
    prog.Use();
    prog.Clear();
    this->frameCount += 1;

    scene->draw(*this);
}

void Renderer::OnWindowResize(int width, int height)
{
    this->window = {width, height};
    glViewport(0, 0, width, height); $gl_chk
}

void Renderer::OnMouseMove(double x, double y)
{
    mouseDelta = {x - mouse.x, y - mouse.y};
    mouse = {x, y};
    mouseDragDelta = mouse - mouseDragStart;
}

void Renderer::OnMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouseDragStart = mouse;
        mouseDragDelta = {0.0f, 0.0f};
    }
    scene->OnMouseButton(button, action, mods);
}
