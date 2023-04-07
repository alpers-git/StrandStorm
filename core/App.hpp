#pragma once
#include <iostream>
#include <Renderer.hpp>
#include <GUIManager.hpp>
#include <EventHandler.hpp>

class App
{
public:
    App(int argc, char *argv[]);

    // Runs the main event loop
    void Run(EventHandler &eventHandler);
private:
    Renderer renderer;
    GUIManager gui;
    Scene scene;
};