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
    hairMesh.build(r.prog);

    surfaceMesh.loadFromFile("resources/suzanne.obj");
    surfaceMesh.build(r.prog);
}

void Scene::draw(const Renderer& r)
{
    glm::vec2 dragDelta = {0.0f, 0.0f};
    if (dragging) {
        const float maxWinDim = (float)std::max(r.window.x, r.window.y);
        dragDelta = (r.mouseDragDelta / maxWinDim) * 3.0f;
        dragDelta.x = -dragDelta.x;
    }
    cam.control({0.0f, 0.0f}, dragDelta, {0.0f, 0.0f});

    r.prog.SetUniform("uTModel", glm::mat4(1.0f));
    r.prog.SetUniform("uTView", cam.view());
    r.prog.SetUniform("uTProj", cam.proj({r.window}));

    hairMesh.draw(r.prog);
    surfaceMesh.draw(r.prog);
}

void Scene::OnMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        cam.dragStart();
        dragging = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        cam.dragStart();
        dragging = false;
    }
}