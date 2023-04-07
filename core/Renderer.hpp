#pragma once

#include <OpenGLProgram.hpp>
#include <stdio.h>
#include <EventHandler.hpp>
#include <Camera.hpp>

class Scene;

class Renderer
{
public:
    void Initialize(Scene& scene);
    void Render(Scene& scene);
private:
    OpenGLProgram prog;
    long int frameCount = 0;
};
