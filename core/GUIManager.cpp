#include <GUIManager.hpp>
#include <EventHandler.hpp>

void StyleColorsAlteredDracula()
{
    auto &colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.05f, 0.05f, 0.085f, .8f};
    colors[ImGuiCol_MenuBarBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Border
    colors[ImGuiCol_Border] = ImVec4{0.44f, 0.37f, 0.61f, 0.99f};
    colors[ImGuiCol_BorderShadow] = ImVec4{0.0f, 0.0f, 0.0f, 0.24f};

    // Text
    colors[ImGuiCol_Text] = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
    colors[ImGuiCol_TextDisabled] = ImVec4{0.5f, 0.5f, 0.5f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.13f, 0.13f, 0.17, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.44f, 0.37f, 0.61f, 0.94f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.84f, 0.58f, 1.0f, 1.0f};
    colors[ImGuiCol_CheckMark] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};

    // Popups
    colors[ImGuiCol_PopupBg] = ImVec4{0.1f, 0.1f, 0.13f, 0.92f};

    // Slider
    colors[ImGuiCol_SliderGrab] = ImVec4{0.44f, 0.37f, 0.61f, 0.94f};
    colors[ImGuiCol_SliderGrabActive] = ImVec4{0.74f, 0.58f, 0.98f, 0.54f};

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.13f, 0.13, 0.17, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.24, 0.24f, 0.32f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4{0.1f, 0.1f, 0.13f, 1.0f};
    colors[ImGuiCol_ScrollbarGrab] = ImVec4{0.16f, 0.16f, 0.21f, 1.0f};
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{0.19f, 0.2f, 0.25f, 1.0f};
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{0.24f, 0.24f, 0.32f, 1.0f};

    // Seperator
    colors[ImGuiCol_Separator] = ImVec4{0.44f, 0.37f, 0.61f, 1.0f};
    colors[ImGuiCol_SeparatorHovered] = ImVec4{0.74f, 0.58f, 0.98f, 1.0f};
    colors[ImGuiCol_SeparatorActive] = ImVec4{0.84f, 0.58f, 1.0f, 1.0f};

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4{0.44f, 0.37f, 0.61f, 0.29f};
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.74f, 0.58f, 0.98f, 0.29f};
    colors[ImGuiCol_ResizeGripActive] = ImVec4{0.84f, 0.58f, 1.0f, 0.29f};

    // Docking
    // colors[ImGuiCol_DockingPreview] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };

    auto &style = ImGui::GetStyle();
    style.TabRounding = 4;
    style.ScrollbarRounding = 9;
    style.WindowRounding = 7;
    style.GrabRounding = 3;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ChildRounding = 4;
}

void GUIManager::Initialize()
{
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;

    // ImGui::StyleColorsDark();
    StyleColorsAlteredDracula();
    ImGui_ImplGlfw_InitForOpenGL(EventHandler::GetInstance().GetWindowPointer(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void GUIManager::Draw()
{
    //place the window in the top left corner
    //printf("window size: %f, %f\n", windowSize.x, windowSize.y);
    NewFrame();
    auto windowSize = ImGui::GetContentRegionAvail();

    ImGui::SetWindowPos("Renderer Controls", ImVec2(windowSize.x/50, windowSize.y/50), ImGuiCond_Once);
    ImGui::SetWindowSize("Renderer Controls", ImVec2(0.9 * windowSize.x,  1.25f * windowSize.y), ImGuiCond_Once);
    ImGui::SetWindowPos("Physics Controls", ImVec2(windowSize.x/50, windowSize.y/10 + 1.25f * windowSize.y), ImGuiCond_Once);
    ImGui::SetWindowSize("Physics Controls", ImVec2(0.9 * windowSize.x, 0.50f * windowSize.y), ImGuiCond_Once);

    ImGui::Begin("Renderer Controls", 0, windowFlags);

    DrawHairMeshControls();
    DrawSurfaceMeshControls();
    DrawLightControls();

    ImGui::End();
    //-------------------------------------------------------------------
    //place the window in the top right corner
    ImGui::Begin("Physics Controls", 0, windowFlags);

    DrawSimulationControls();
    DrawRodParameters();

    ImGui::End();
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIManager::DrawHairMeshControls()
{
   if(ImGui::CollapsingHeader("Hair Mesh Controls", ImGuiTreeNodeFlags_DefaultOpen))
   {
        ImGui::Checkbox("Show", &scene->hairMesh.show);
        ImGui::SameLine();
        ImGui::Checkbox("Shadows", &scene->hairMesh.shadowsEnable);
        ImGui::SeparatorText("Hair Mesh Material");
        ImGui::ColorEdit4("Color##0", &scene->hairMesh.color[0]);
        ImGui::ColorEdit4("Ambient##0", &scene->hairMesh.ambient[0]);
        ImGui::ColorEdit4("Specular##0", &scene->hairMesh.specular[0]);
        ImGui::InputFloat("Shininess##0", &scene->hairMesh.shininess);
        // ImGui::SameLine();
        // ImGui::Checkbox("Show Control Hairs", nullptr);
        // ImGui::InputInt("Guide hair count", nullptr);
        // ImGui::InputFloat("Guide hair length", nullptr);
   }
}

void GUIManager::DrawSurfaceMeshControls()
{
    if(ImGui::CollapsingHeader("Surface Mesh Controls", ImGuiTreeNodeFlags_DefaultOpen))
    {
        //ImGui::Checkbox("Show Surface Mesh", nullptr);
        ImGui::SeparatorText("Surface Mesh Material");
        ImGui::ColorEdit3("Specular", &scene->surfaceMesh.material.specular[0]);
        ImGui::ColorEdit3("Diffuse", &scene->surfaceMesh.material.diffuse[0]);
        ImGui::ColorEdit3("Ambient", &scene->surfaceMesh.material.ambient[0]);
    }
}

void GUIManager::DrawLightControls()
{
    if(ImGui::CollapsingHeader("Light Controls", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SeparatorText("Light 1");
        auto width = ImGui::GetContentRegionAvail().x;
        ImGui::PushItemWidth(width * 0.10f);
        ImGui::DragFloat("Intensity", &scene->light.intensity, 0.01f, 0.0f, 1.0f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(width * 0.55f);
        ImGui::ColorEdit3("Color##1", &scene->light.color[0]);
        ImGui::PopItemWidth();
        if(ImGui::DragFloat3("Direction", &scene->light.dir[0], 0.01f))
            scene->light.opacityShadowMaps.dirty = true;
        if(ImGui::CollapsingHeader("Op. Shadow Map", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static float dk = scene->light.opacityShadowMaps.dk;
            if (ImGui::DragFloat("dk", &dk, 0.001f, 0.0001f, 1.0f))
            {
                scene->light.opacityShadowMaps.dk = std::max(dk, 0.0001f);
                scene->light.opacityShadowMaps.dirty = true;
            }
            
            auto width = ImGui::GetContentRegionAvail().x;
            //Show the shadow maps
            ImGui::BeginGroup();
            ImGui::Text("Depth Map");
            ImGui::Image((void*)scene->light.opacityShadowMaps.depthTex->glID, 
                ImVec2(width/2, width/2));
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("Opacity Map");
            ImGui::Image((void*)scene->light.opacityShadowMaps.opacitiesTex->glID, 
                ImVec2(width/2, width/2));
            ImGui::EndGroup();
        }
        // ImGui::SeparatorText("Light 2");
        // ImGui::ColorEdit3("Color", &scene->light2.color[0]);
        // ImGui::InputFloat3("Position", &scene->light2.position[0]);
    }
}

void GUIManager::DrawSimulationControls()
{
    if(ImGui::CollapsingHeader("Simulation Controls", ImGuiTreeNodeFlags_DefaultOpen))
    {
        float dt =  physicsIntegrator->getDt();
        if(ImGui::DragFloat("dt", &dt, 0.001f, 0.0f, 1.0f)) 
            physicsIntegrator->setDt(dt);
        int numSteps = physicsIntegrator->getNumSteps();
        if(ImGui::InputInt("numSteps", &numSteps , 1, 1, ImGuiInputTextFlags_EnterReturnsTrue)) 
            physicsIntegrator->setNumSteps(numSteps);
    }
}

void GUIManager::DrawRodParameters()
{
    if(ImGui::CollapsingHeader("Elactic Rods", 0))
    {
        static Eigen::Vector3f gravity(0.0f, 0.0f, 0.0f);
        if(ImGui::DragFloat3("gravity", &gravity[0], 0.001f, -10.0f, 10.0f))
            scene->setGravity(gravity);

        auto width = ImGui::GetContentRegionAvail().x;
        ImGui::PushItemWidth(width * 0.45f);
        static float drag = 0.0f;
        if(ImGui::DragFloat("drag", &drag, 0.0001f, 0.0f, 100.0f, "%.4f"))
            scene->setDrag(drag);
        ImGui::PopItemWidth();
    }
}

void GUIManager::Terminate()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUIManager::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}