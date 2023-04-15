#include <Scene.hpp>
#include <Renderer.hpp>

Scene::Scene()
{
    cam.pos = {0.0f, 0.0f, 2.0f};
    cam.distance = 3.0f;
}

void Scene::init(const Renderer& r)
{
    hairMesh.loadFromFile("resources/suzanne.obj");
    hairMesh.build(r.hairProg);

    surfaceMesh.loadFromFile("resources/suzanne.obj");
    surfaceMesh.build(r.surfaceProg);

    light.hairShadowTexture = std::make_shared<ShadowTexture>(glm::uvec2(1024, 1024));
}

glm::mat4 Scene::Light::CalculateLightSpaceMatrix() const
{
    const glm::mat4 lightProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.01f, 20.f);
    const glm::mat4 lightView = glm::lookAt(-glm::normalize(dir) * 20.f,
                                            glm::vec3(0.0f, 0.0f, 0.0f),
                                            glm::vec3(0.0f, 0.0f, 1.0f));
    return lightProjection * lightView;
}
