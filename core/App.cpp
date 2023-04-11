#include <App.hpp>

App::App(int argc, char *argv[])
{
    scene = std::make_shared<Scene>();
    
    renderer.scene = scene;
    renderer.Initialize();

    gui.scene = scene;
    gui.Initialize();
}

void App::Run(EventHandler &eventHandler)
{
    while (eventHandler.IsRunning()) {
        eventHandler.SwapBuffers();
        eventHandler.DispatchEvents(renderer);
        renderer.Render();
        gui.Draw();
    }

    // Clean up here
    gui.Terminate();
}