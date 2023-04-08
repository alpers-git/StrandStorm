#pragma once

#include <OpenGLProgram.hpp>
#include <EventHandler.hpp>
#include <Camera.hpp>

class Scene;

class Renderer
{
public:
    OpenGLProgram prog;
    long int frameCount = 0;
    glm::ivec2 window;
    glm::vec2 mouse;
    glm::vec2 mouseDelta;
    glm::vec2 mouseDragDelta;
    glm::vec2 mouseDragStart;
    std::shared_ptr<Scene> scene;

    void Initialize();
    void Render();
    void OnWindowResize(int width, int height);
    void OnMouseMove(double x, double y);
    void OnMouseButton(int button, int action, int mods);
};
