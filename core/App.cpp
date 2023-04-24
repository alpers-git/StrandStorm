#include <App.hpp>
#include <future>
#include <Stats.hpp>

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
        auto start = std::chrono::high_resolution_clock::now();

        auto f = std::async(std::launch::async | std::launch::deferred, [&] {
            auto startP = std::chrono::high_resolution_clock::now();

            physicsIntegrator->Integrate();

            auto endP = std::chrono::high_resolution_clock::now();

            stats::lastPhysicsTime = std::chrono::duration_cast<std::chrono::milliseconds>(endP - startP).count() / 1000.0f;
            stats::avgPhysicsTime = stats::avgPhysicsTime * 0.99f + stats::lastPhysicsTime * 0.01f; // rolling average
        }); // Run the physics integrator in a separate thread

        eventHandler.SwapBuffers();
        eventHandler.DispatchEvents(renderer);

        auto startR = std::chrono::high_resolution_clock::now();
        renderer.Render();
        auto endR = std::chrono::high_resolution_clock::now();
        stats::lastRenderTime = std::chrono::duration_cast<std::chrono::milliseconds>(endR - startR).count() / 1000.0f;
        stats::avgRenderTime = stats::avgRenderTime * 0.99f + stats::lastRenderTime * 0.01f; // rolling average

        auto end = std::chrono::high_resolution_clock::now();
        stats::lastFrameTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0f;
        stats::avgFrameTime = stats::avgFrameTime * 0.99f + stats::lastFrameTime * 0.01f; // rolling average

        gui.Draw();
        f.wait();
    }

    // Clean up here
    gui.Terminate();
}
