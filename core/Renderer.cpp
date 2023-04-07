#include <Renderer.hpp>
#include <Scene.hpp>

void Renderer::Initialize(Scene& scene)
{
    prog.CreatePipelineFromFiles("shaders/simple.vert", "shaders/simple.frag");
    prog.Use();
    scene.init();
}

void Renderer::Render(Scene& scene)
{
    EventHandler::GetInstance().SwapBuffers();
    this->frameCount += 1;
    
    const glm::vec3 clearColor1(0.09f, 0.30f, 0.55f);
    const glm::vec3 clearColor2(1.0f, 0.76f, 0.03f);
    float time = EventHandler::GetInstance().GetTime() * 10.0f;
    float value = (sin(time) + 1.0f) / 2.0f;
    prog.SetClearColor(glm::vec4(glm::mix(
        clearColor1,
        clearColor2,
        value), 1.0f));
    prog.Clear();

    scene.draw(prog);
}