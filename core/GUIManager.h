#pragma once

#include <iostream>
#include <memory>

#include <imgui.h>
#include <glad/glad.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <Scene.h>

namespace ssCore
{
    class GUIManager
    {
    public:
        GUIManager(std::shared_ptr<Scene> scene);
        ~GUIManager();

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

        ImGuiWindowFlags windowFlags;
		std::string windowName = "GUI";

        private:
        std::shared_ptr<Scene> scene;

        void NewFrame();
        //Other ui components draw functions here
    };
} // namespace ssCore