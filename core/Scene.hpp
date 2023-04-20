#pragma once

#include <Mesh.hpp>
#include <Camera.hpp>
#include <ElasticRod.hpp>

class Renderer;

class Scene
{
public:
    HairMesh hairMesh;
    SurfaceMesh surfaceMesh;
    std::vector<ElasticRod> rods;
    Camera cam;
    struct Light {
        glm::vec3 dir = glm::vec3(0.0f, -1.0f, -0.1f);
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        float intensity = 1.0f;
        std::shared_ptr<ShadowTexture> shadowTexture;

        struct {
            std::shared_ptr<DepthTexture> depthTex;
            std::shared_ptr<RenderedTexture> opacitiesTex;
            float dk = 0.015f; //distance between layers
            bool dirty = true;
        } opacityShadowMaps;
        glm::mat4 CalculateLightSpaceMatrix() const;
        glm::mat4 CalculateLightTexSpaceMatrix() const;
    } light;

    Scene();

    // Called by Renderer::Initialize()
    void init(const Renderer& r);
private:
    /* data */
};