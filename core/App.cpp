#include <App.hpp>

App::App(int argc, char *argv[])
{
    scene = std::make_shared<Scene>();
    
    renderer.scene = scene;
    renderer.Initialize();

    physicsIntegrator = std::make_shared<PhysicsIntegrator>();
    physicsIntegrator->scene = scene;
    physicsIntegrator->Initialize();

    gui.scene = scene;
    gui.physicsIntegrator = physicsIntegrator;
    gui.Initialize();
}

void App::Run(EventHandler &eventHandler)
{
    while (eventHandler.IsRunning()) {
        physicsIntegrator->Integrate();
        eventHandler.SwapBuffers();
        eventHandler.DispatchEvents(renderer);
        renderer.Render();
        gui.Draw();
    }

    // Clean up here
    gui.Terminate();
}