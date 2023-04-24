#pragma once

#include <Mesh.hpp>
#include <Camera.hpp>
#include <ElasticRod.hpp>
#include <Collider.hpp>
#include <VoxelGrid.hpp>

class Renderer;

struct SceneObject
{
    SurfaceMesh mesh;
    std::shared_ptr<Collider> collider;
    
    // Transform parameters
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    // Sets position, rotation and scale from given arguments
    void setTransform(const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale);
    // Updates position, rotation and scale from stored variables
    void setTransform();
};

class Scene
{
public:
    HairMesh hairMesh;
    std::shared_ptr<SceneObject> surface, dummy;
    std::vector<std::shared_ptr<SceneObject>> sceneObjects;
    std::vector<ElasticRod> rods;
    std::shared_ptr<VoxelGrid> voxelGrid;
    Camera cam;
    struct Light {
        glm::vec3 dir = glm::vec3(-3.7f, 0.5f, -5.1f);
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
    // Resets entire simulation
    void reset();
private:
    /* data */
};