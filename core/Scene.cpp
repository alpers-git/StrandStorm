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
// void Scene::OnMouseButton(int button, int action, int mods)
// {
//     if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
//     {
//         cam.dragStart();
//         dragging = true;
//     }
//     else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
//     {
//         cam.dragStart();
//         dragging = false;
//     }
// }