#pragma once
#include <iostream>
#include <Renderer.h>
#include <EventHandler.h>

namespace ssCore
{
    template <class R /*, class G, class P*/>
    class Application
    {
    public:
        Application(int argc, char *argv[])
        {
            // Initialize
            printf("Running StrandStrom\n");

            // We can parse command line arguments here using something like argparse orother libraries
        }
        //~Application(){}

        void Run()
        {
            auto &eventHandler = EventHandler::GetInstance();
            eventHandler.InitAndCreateWindow(1280, 720, "StrandStrom");

            // Initialize here

            renderer->Initialize();

            // Application loop
            while (eventHandler.IsRunning())
            {
                // Update physics here

                // Render the scene here
                renderer->Render();

                // Draw GUI here

                // Handle events here
                eventHandler.DispatchEvents();
            }

            // Clean up here
        }

    private:
        std::unique_ptr<R> renderer;
        // std::unique_ptr<G> guiManager;
        // std::unique_ptr<P> physicsIntegrator;
        // std::shared_ptr<Scene> scene;
    };
} // namespace ssCore