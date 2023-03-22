#pragma once

#include <iostream>
#include <imgui.h>
#include <glad/glad.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace ssCore
{
    class GUIManager
    {
    public:
        GUIManager();
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
        void NewFrame();
    };
} // namespace ssCore