#include <App.hpp>

App::App(int argc, char *argv[])
{
    renderer.Initialize(scene);
    gui.Initialize();
}

void App::Run(EventHandler &eventHandler)
{
    
    while (eventHandler.IsRunning()) {
        eventHandler.SwapBuffers();
        // Update physics here

        // Render the scene here
        renderer.Render(scene);

        // Draw GUI here
        gui.Draw();

        // Handle events here
        eventHandler.DispatchEvents();
    }

    // Clean up here
    gui.Terminate();
}