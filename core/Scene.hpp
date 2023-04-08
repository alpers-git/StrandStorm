#pragma once

#include <Mesh.hpp>
#include <Camera.hpp>

class Renderer;

class Scene
{
public:
    HairMesh hairMesh;
    Camera cam;
    
    Scene();

    // Called by Renderer::Initialize()
    void init(const Renderer& r);
    void draw(const Renderer& r);
    void OnMouseButton(int button, int action, int mods);
private:
    /* data */
    bool dragging = false;
};