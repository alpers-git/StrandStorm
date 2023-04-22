#pragma once

#include <iostream>
#include <memory>

#include <imgui.h>
#include <glad/glad.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <Scene.hpp>
#include <PhysicsIntegrator.hpp>

class GUIManager
{
public:
    /*
    * Called before the application loop starts
    */
    void Initialize();

    /*
    * Called every frame in the application loop draws the GUI using ImGui
    */
    void Draw();


    /*
    * Called after the application loop ends cleans up ImGui
    */
    void Terminate();

    void SetScaling(int scalingFactor);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove;
    //std::string windowName = "GUI";

    std::shared_ptr<Scene> scene;   
    std::shared_ptr<PhysicsIntegrator> physicsIntegrator;
private:
    ImFont* font = nullptr;
    int scalingFactor = 1;

    void NewFrame();
    //Other ui components draw functions here
    void DrawHairMeshControls();
    void DrawSurfaceMeshControls();
    void DrawLightControls();

    void DrawSimulationControls();
    void DrawRodParameters();
};