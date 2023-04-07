#include <Scene.hpp>

Scene::Scene()
{
    cam.pos = {0.0f, 0.0f, -2.0f};
}

void Scene::init()
{
    hairMesh.loadFromFile("resources/suzanne.obj");
    hairMesh.build();
}

void Scene::draw(const OpenGLProgram &prog)
{
    prog.SetUniform("uTModel", glm::mat4(1.0f));
    prog.SetUniform("uTView", cam.view());
    prog.SetUniform("uTProj", cam.proj());

    hairMesh.draw(prog);
}