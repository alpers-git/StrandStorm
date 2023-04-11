#pragma once

#include <Mesh.hpp>
#include <Camera.hpp>

class Renderer;

class Scene
{
public:
    HairMesh hairMesh;
    SurfaceMesh surfaceMesh;
    Camera cam;
    struct Light {
        glm::vec3 pos = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float intensity = 1.0f;
    } light;
    
    Scene();

    // Called by Renderer::Initialize()
    void init(const Renderer& r);
private:
    /* data */
};