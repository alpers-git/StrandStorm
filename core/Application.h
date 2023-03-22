#pragma once
#include <iostream>
#include <Renderer.h>
#include <GUIManager.h>
#include <EventHandler.h>

namespace ssCore
{
    template <class R /*, class G, class P*/>
    class Application
    {
    public:
        Application(int argc, char *argv[])
        {
            // Initialize the application and its subsystems
            printf("Running StrandStrom\n");
            renderer = std::make_unique<R>();
            gui = std::make_unique<GUIManager>();

            // We can parse command line arguments here using something like argparse orother libraries
        }
        //~Application(){}

        void Run()
        {
            auto &eventHandler = EventHandler::GetInstance();
            eventHandler.InitAndCreateWindow(1280, 720, "StrandStrom");

            // Initialize here
            renderer->Initialize();
            gui->Initialize();

            // Application loop
            while (eventHandler.IsRunning())
            {
                // Update physics here

                // Render the scene here
                renderer->Render();

                // Draw GUI here
                gui->Draw();
                // Handle events here
                eventHandler.DispatchEvents();
            }

            // Clean up here
            renderer->Terminate();
        }

    private:
        std::unique_ptr<R> renderer;
        std::unique_ptr<GUIManager> gui;
        // std::unique_ptr<P> physicsIntegrator;
        // std::shared_ptr<Scene> scene;
    };
} // namespace ssCore