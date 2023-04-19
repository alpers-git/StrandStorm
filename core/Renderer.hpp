#pragma once

#include <OpenGLProgram.hpp>
#include <EventHandler.hpp>
#include <Camera.hpp>
#include <ComputeShader.hpp>

class Scene;

class Renderer
{
public:
    ComputeShader csHair;
    OpenGLProgram hairProg = {"hair"};
    OpenGLProgram surfaceProg = {"surface"};
    OpenGLProgram shadowProg = {"shadow"};
    OpenGLProgram opacityShadowProg = {"opacity shadow"};

    long int frameCount = 0;
    glm::ivec2 windowSize;

    struct mouseInteraction {
        bool leftButton = false;
        bool rightButton = false;
        bool middleButton = false;

        glm::vec2 lastPos;
        glm::vec2 delta;

    } mouseInteraction;

    std::shared_ptr<Scene> scene;

    void Initialize();
    void Render();
    void OnWindowResize(int width, int height);
    void OnMouseMove(double x, double y);
    void OnMouseButton(int button, int action, int mods);

private:
    void RenderHairs();
    void RenderSurfaces();

    void RenderFirstPass();
    void RenderMainPass();
};
