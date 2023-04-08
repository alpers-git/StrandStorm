#pragma once

#include <Mesh.hpp>
#include <Camera.hpp>

class Scene
{
public:
    HairMesh hairMesh;
    Camera cam;
    
    Scene();

    // Called by Renderer::Initialize()
    void init(const OpenGLProgram& prog);
    void draw(const OpenGLProgram& prog);
private:
    /* data */
};