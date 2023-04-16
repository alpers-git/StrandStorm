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

    TextureParams params;
    params.wrapS = GL_CLAMP_TO_BORDER;
    params.wrapT = GL_CLAMP_TO_BORDER;
    params.internalFormat = GL_DEPTH_COMPONENT32;
    params.format = GL_DEPTH_COMPONENT;
    params.type = GL_FLOAT;
    light.hairShadowTexture = std::make_shared<ShadowTexture>(
        glm::uvec2(1024, 1024), GL_TEXTURE5, 
        params);
}

glm::mat4 Scene::Light::CalculateLightSpaceMatrix() const
{
    const glm::mat4 lightProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.01f, 10.f);
    const glm::mat4 lightView = glm::lookAt(-glm::normalize(dir) * 10.f,
                                            glm::vec3(0.0f, 0.0f, 0.0f),
                                            glm::vec3(0.0f, 0.0f, 1.0f));
    return lightProjection * lightView;
}
