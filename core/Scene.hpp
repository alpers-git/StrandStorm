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
        glm::vec3 dir = glm::vec3(0.0f, -1.0f, -0.1f);
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float intensity = 1.0f;
        std::shared_ptr<ShadowTexture> shadowTexture;

        struct {
            std::shared_ptr<ShadowTexture> depthTex;
            std::shared_ptr<RenderedTexture> opacitiesTex;
            float dk = 0.1f; //distance between layers
        } opacityShadowMaps;
        glm::mat4 CalculateLightSpaceMatrix() const;
    } light;
    
    Scene();

    // Called by Renderer::Initialize()
    void init(const Renderer& r);
private:
    /* data */
};