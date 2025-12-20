#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <tinyfiledialogs.h>

#include <App.hpp>
#include <AppContext.hpp>
#include <CubeRenderer.hpp>
#include <LazReader.hpp>
#include <OrbitalCamera.hpp>

namespace UserInterface {

    static int selectedColorRampIndex = 0;

    void SetCustomTheme() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;
        ImGui::StyleColorsDark();

        // ROUNDING
        style.WindowRounding = 4.0f;
        style.FrameRounding = 4.0f;
        style.ChildRounding = 6.0f;
        style.PopupRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.ScrollbarRounding = 6.0f;

        // WINDOW
        style.WindowBorderSize = 0.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);    // WINDOW TITLE CENTERED
        
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
        style.ScrollbarSize = 18.0f;
        style.FramePadding = ImVec2(10.0f, 6.0f);
        style.FrameBorderSize = 0.0f;
        style.GrabMinSize = 10.0f;

        // CALCULATE ACCENT VALUES
        // glm::vec3 AccentBase = glm::vec3(0.000f, 0.729f, 0.380f);   // GREEN
        // glm::vec3 AccentBase = glm::vec3(0.608f, 0.000f, 0.620f);   // VIOLET
        glm::vec3 AccentBase = glm::vec3(0.000f, 0.621f, 0.464f);   // SEA-GREEN
        glm::vec3 AccentDark = AccentBase * 0.90f;
        glm::vec3 AccentLight = AccentBase * 1.15f;

        // TEXT
        colors[ImGuiCol_Text]                       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled]               = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

        // TITLE (PANEL)
        colors[ImGuiCol_TitleBg]                    = ImVec4(AccentBase.r, AccentBase.g, AccentBase.b, 1.00f);
        colors[ImGuiCol_TitleBgActive]              = ImVec4(AccentBase.r, AccentBase.g, AccentBase.b, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]           = ImVec4(AccentBase.r, AccentBase.g, AccentBase.b, 1.00f);

        // WINDOW BACKGROUNDS
        colors[ImGuiCol_WindowBg]                   = ImVec4(0.06f, 0.06f, 0.06f, 0.95f);
        colors[ImGuiCol_ChildBg]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]                    = ImVec4(0.08f, 0.08f, 0.08f, 0.95f);

        // HEADERS
        colors[ImGuiCol_Header]                     = ImVec4(AccentDark.r, AccentDark.g, AccentDark.b, 0.31f);
        colors[ImGuiCol_HeaderHovered]              = ImVec4(AccentDark.r, AccentDark.g, AccentDark.b, 0.80f);
        colors[ImGuiCol_HeaderActive]               = ImVec4(AccentDark.r, AccentDark.g, AccentDark.b, 0.65f);

        // SLIDERS / FRAMES
        colors[ImGuiCol_FrameBg]                    = ImVec4(0.16f, 0.16f, 0.16f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]             = ImVec4(0.26f, 0.26f, 0.26f, 0.40f);
        colors[ImGuiCol_FrameBgActive]              = ImVec4(0.26f, 0.26f, 0.26f, 0.67f);

        colors[ImGuiCol_SliderGrab]                 = ImVec4(AccentBase.r, AccentBase.g, AccentBase.b, 1.00f);
        colors[ImGuiCol_SliderGrabActive]           = ImVec4(AccentLight.r, AccentLight.g, AccentLight.b, 1.00f);

        // BUTTONS
        colors[ImGuiCol_Button]                     = ImVec4(0.16f, 0.16f, 0.16f, 0.54f);
        colors[ImGuiCol_ButtonHovered]              = ImVec4(0.26f, 0.26f, 0.26f, 0.40f);
        colors[ImGuiCol_ButtonActive]               = ImVec4(0.26f, 0.26f, 0.26f, 0.67f);

        // CHECK MARKS
        colors[ImGuiCol_CheckMark]                  = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);

        // SCROLLBAR
        colors[ImGuiCol_ScrollbarBg]                = ImVec4(0.25f, 0.25f, 0.25f, 0.60f);
        colors[ImGuiCol_ScrollbarGrab]              = ImVec4(0.53f, 0.53f, 0.53f, 0.30f);
        colors[ImGuiCol_ScrollbarGrabHovered]       = ImVec4(0.53f, 0.53f, 0.53f, 0.40f);
        colors[ImGuiCol_ScrollbarGrabActive]        = ImVec4(0.53f, 0.53f, 0.53f, 0.60f);

        // RESIZE
        colors[ImGuiCol_ResizeGrip]                 = ImVec4(AccentBase.r, AccentBase.g, AccentBase.b, 0.20f);
        colors[ImGuiCol_ResizeGripHovered]          = ImVec4(AccentBase.r, AccentBase.g, AccentBase.b, 0.67f);
        colors[ImGuiCol_ResizeGripActive]           = ImVec4(AccentBase.r, AccentBase.g, AccentBase.b, 0.95f);

        // SEPERATORS (WINDOW RESIZE ALSO)
        style.Colors[ImGuiCol_Separator]            = ImVec4(AccentBase.r, AccentBase.g, AccentBase.b, 1.00f);
        style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(AccentBase.r, AccentBase.g, AccentBase.b, 0.78f);
        style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(AccentBase.r, AccentBase.g, AccentBase.b, 1.00f);
    }

    void DrawFileSelectionSettings(Application::AppContext* appContext) {
        CreateControlSection("File", true, appContext, [&]() {
            ImGuiStyle& style = ImGui::GetStyle();

            const float buttonSpacing = 4.0f;
            const float selectButtonHeight = ImGui::GetTextLineHeight() + style.FramePadding.y * 2.0f;
            const ImVec2 closeButtonSize = ImVec2(selectButtonHeight, selectButtonHeight);
            const float selectButtonWidth = ImGui::GetContentRegionAvail().x - closeButtonSize.x - buttonSpacing;
            const char* selectButtonLabel = appContext->filepath.empty() ? "Select File..." : appContext->filepath.c_str();

            bool isButtonDisabled = appContext->isReadingFlag.load(std::memory_order_acquire);
            ImGui::BeginDisabled(isButtonDisabled);
            if (ImGui::Button("##SELECT_FILE_BUTTON", ImVec2(selectButtonWidth, selectButtonHeight))) {
                const char* filters[] = { "*.las", "*.laz" };
                const char* selected = tinyfd_openFileDialog(
                    "Select a file", "",
                    2, // NUMBER OF FILTERS
                    filters,
                    ".LAZ and .LAS files",
                    0 // DO NOT ALLOW MULTIPLE SELECTIONS
                );
                if (selected) {
                    appContext->filepath = selected;

                    std::shared_ptr<CustomReader::LazReader> reader = std::make_shared<CustomReader::LazReader>(
                        appContext->filepath,
                        appContext->cubeRenderer.get(),
                        2 // DECIMATION STEP
                    );
                    std::shared_ptr<LazHeader> header = reader->GetHeader(); 

                    // UPDATE CAMERA BOUNDING BOX
                    glm::vec3 minDistance(header->minX, header->minY, header->minZ);
                    glm::vec3 maxDistance(header->maxX, header->maxY, header->maxZ);
                    glm::vec3 center = 0.5f * (minDistance + maxDistance);
                    float radius = 0.5f * glm::length(maxDistance - minDistance);
                    appContext->freeCamera->UpdateBounds(glm::vec3(0.0f), radius);
                    appContext->orbitalCamera->UpdateBounds(glm::vec3(0.0f), radius);

                    // UPDATE GPU INSTANCE BUFFER SIZES
                    appContext->cubeRenderer->Clear();
                    appContext->cubeRenderer->UpdateBufferSize(header->pointCount());

                    // READ LAS/LAZ FILE DATA (SEPERATE THREAD)
                    std::thread([appContext, reader]() {
                        appContext->isReadingFlag.store(true, std::memory_order_release);
                        
                        // NOTE: CANNOT UPDATE OPENGL BUFFERS OUTSIDE OF MAIN THREAD
                        reader->ReadPointData();

                        appContext->doneReadingFlag.store(true, std::memory_order_release);
                    }).detach();
                }
            }

            // SELECTION BUTTON LABEL (LEFT-ALIGNED)
            ImVec2 textSize = ImGui::CalcTextSize(selectButtonLabel);
            ImVec2 buttonMin = ImGui::GetItemRectMin();
            ImVec2 buttonMax = ImGui::GetItemRectMax();
            buttonMax.x = buttonMax.x - style.FramePadding.x;
            float textX = buttonMin.x + style.FramePadding.x;
            float textY = buttonMin.y + (selectButtonHeight - textSize.y) * 0.5f;
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->PushClipRect(buttonMin, buttonMax, true);
            drawList->AddText(ImVec2(textX, textY), ImGui::GetColorU32(ImGuiCol_Text), selectButtonLabel);
            drawList->PopClipRect();

            ImGui::EndDisabled();

            ImGui::SameLine(0.0f, buttonSpacing);
            ImGui::BeginDisabled(appContext->filepath.empty() || isButtonDisabled);

            // FILE DESELECT BUTTON
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.15f, 0.15f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70f, 0.20f, 0.20f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.85f, 0.25f, 0.25f, 1.0f));

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, style.FramePadding.y));
            if (ImGui::Button("X")) {
                appContext->filepath.clear();
                appContext->cubeRenderer->Clear();
            }
            ImGui::PopStyleVar();

            ImGui::PopStyleColor(3);
            ImGui::EndDisabled();
        });
    }

    void DrawCubeSettings(Application::AppContext* appContext) {
        CreateControlSection("Cube", true, appContext, [&]() {
            // GLOBAL SCALE
            ImGui::SliderFloat("Global Scale", &appContext->globalScale, 0.05f, 1.0f);

            // COLOR RAMP (GRADIENT)
            if (ImGui::Combo("Gradient", &selectedColorRampIndex, Data::ColorRampNames, IM_ARRAYSIZE(Data::ColorRampNames))) {
                Data::ColorRampType selectedRamp = static_cast<Data::ColorRampType>(selectedColorRampIndex);
                appContext->cubeRenderer->UpdateColorRamp(selectedRamp);
            }
        });
    }

    void DrawOrbitalCameraSettings(Application::AppContext* appContext) {
        CreateControlSection("Orbital Camera", false, appContext, [&]() {
            // CAMERA ROTATION SPEED
            ImGui::SliderFloat("Speed##ORBITAL_CAMERA", &appContext->orbitalCamera->GetRotationSpeed(), 0.0f, 50.0f);
            
            // CAMERA ZOOM
            ImGui::SliderFloat("Zoom", 
                &appContext->orbitalCamera->GetTargetZoom(), 
                appContext->orbitalCamera->GetMinZoom(), 
                appContext->orbitalCamera->GetMaxZoom()
            );
        });
    }

    void DrawFreeCameraSettings(Application::AppContext* appContext) {
        CreateControlSection("Free Camera", false, appContext, [&]() {
            // CAMERA MOVEMENT SPEED
            ImGui::SliderFloat("Speed##FREE_CAMERA", &appContext->freeCamera->GetSpeedFactor(), 0.05f, 1.0f);

            // FREE CAMERA ACCELERATION SPEED
            ImGui::SliderFloat("Acceleration##FREE_CAMERA", &appContext->freeCamera->GetAccelerationFactor(), 0.0f, 1.0f);
        });
    }

    void RenderMainPanel(Application::AppContext* appContext) {
        ImVec2 minSize(float(MINIMUM_WINDOW_WIDTH) / 2.0f, float(MINIMUM_WINDOW_HEIGHT) / 2.0f);
        ImVec2 maxSize(FLT_MAX, FLT_MAX);
        ImGui::SetNextWindowSize(minSize, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
        ImGui::Begin("##CONTROL_PANEL", nullptr, ImGuiWindowFlags_NoCollapse);

        DrawFileSelectionSettings(appContext);
        DrawCubeSettings(appContext);
        DrawOrbitalCameraSettings(appContext);
        DrawFreeCameraSettings(appContext);

        ImGui::End();
    }

}