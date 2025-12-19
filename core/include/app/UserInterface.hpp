#pragma once

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
#include <CustomReader.hpp>
#include <OrbitalCamera.hpp>

namespace UserInterface {

    void SetCustomTheme();

    template <typename Callable>
    inline void CreateControlSection(const char* label, bool defaultOpen, Application::AppContext* appContext, Callable&& content) {
        ImGuiStyle& style = ImGui::GetStyle();

        ImGuiID id = ImGui::GetID(label);
        bool isOpen = ImGui::GetStateStorage()->GetBool(id, defaultOpen);
        ImVec4 baseColor = style.Colors[ImGuiCol_Header];
        ImVec4 activeColor = style.Colors[ImGuiCol_HeaderActive];
        ImGui::PushStyleColor(ImGuiCol_Header, isOpen ? activeColor : baseColor);
        ImGui::PushFont(appContext->fontBold);
        if (ImGui::CollapsingHeader(label, isOpen ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None)) {
            content();
        }
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }

    void DrawFileSelectionSettings(Application::AppContext* appContext);

    void DrawCubeSettings(Application::AppContext* appContext);

    void DrawOrbitalCameraSettings(Application::AppContext* appContext);

    void RenderMainPanel(Application::AppContext* appContext);

}