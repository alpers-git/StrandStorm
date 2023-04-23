#include <App.hpp>
#include <future>

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
        std::async(std::launch::async | std::launch::deferred, [&] { physicsIntegrator->Integrate(); }); // Run the physics integrator in a separate thread
        eventHandler.SwapBuffers();
        eventHandler.DispatchEvents(renderer);
        renderer.Render();
        gui.Draw();
    }

    // Clean up here
    gui.Terminate();
}