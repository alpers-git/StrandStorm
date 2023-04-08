#include <Renderer.hpp>
#include <Scene.hpp>

void Renderer::Initialize(Scene& scene)
{
    glLineWidth(3.0f); $gl_chk
    glEnable(GL_DEPTH_TEST); $gl_chk

    prog.CreatePipelineFromFiles("shaders/simple.vert", "shaders/simple.frag");
    prog.Use();

    scene.init(prog);
}

void Renderer::Render(Scene& scene)
{
    prog.Use();

    this->frameCount += 1;
    prog.SetClearColor({0.0f, 0.0f, 0.0f, 0.0f});
    prog.Clear();

    scene.draw(prog);
}