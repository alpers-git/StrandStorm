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
            // We can parse command line arguments here using something like argparse orother libraries

            //Init scene from arguments
            scene = std::make_shared<Scene>();
            // Initialize the application and its subsystems
            printf("Running StrandStrom\n");
            renderer = std::make_unique<R>(scene);
            gui = std::make_unique<GUIManager>(scene);

        }
        //~Application(){}

        void Run()
        {
            auto &windowEventHandler = EventHandler::GetInstance();
            windowEventHandler.InitAndCreateWindow(1280, 720, "StrandStrom");

            // Initialize here
            renderer->Initialize();
            gui->Initialize();

            // Application loop
            while (windowEventHandler.IsRunning())
            {
                // Update physics here

                // Render the scene here
                renderer->Render();

                // Draw GUI here
                gui->Draw();
                // Handle events here
                windowEventHandler.DispatchEvents();
            }

            // Clean up here
            renderer->Terminate();
        }

    private:
        std::unique_ptr<R> renderer;
        std::unique_ptr<GUIManager> gui;
        // std::unique_ptr<P> physicsIntegrator;

        std::shared_ptr<Scene> scene;
    };
} // namespace ssCore