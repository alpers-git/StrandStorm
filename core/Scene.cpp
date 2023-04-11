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

    // r.hairProg.SetUniform("uTModel", glm::mat4(1.0f));
    // r.hairProg.SetUniform("uTView", cam.view());
    // r.hairProg.SetUniform("uTProj", cam.proj({r.window}));

    // hairMesh.draw(r.hairProg);

    //-----------------------------------------------------
    glm::mat4 to_screen_space = cam.proj({r.window}) * cam.view();
    glm::mat4 to_view_space = cam.view();
    glm::mat3 normals_to_view_space = glm::mat3(glm::transpose(glm::inverse(to_view_space)));
    r.surfaceProg.SetUniform("to_screen_space", to_screen_space);
    r.surfaceProg.SetUniform("to_view_space", to_view_space);
    r.surfaceProg.SetUniform("normals_to_view_space", normals_to_view_space);

    surfaceMesh.draw(r.surfaceProg);
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