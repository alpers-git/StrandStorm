#pragma once
#include <iostream>
#include <Renderer.hpp>
#include <GUIManager.hpp>
#include <EventHandler.hpp>
#include <PhysicsIntegrator.hpp>

class App
{
public:
    App(int argc, char *argv[]);

    // Runs the main event loop
    void Run(EventHandler &eventHandler);
private:
    Renderer renderer;
    GUIManager gui;
    std::shared_ptr<PhysicsIntegrator> physicsIntegrator;
    std::shared_ptr<Scene> scene;
};