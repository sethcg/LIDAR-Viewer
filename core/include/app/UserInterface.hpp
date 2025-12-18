#pragma once

#include <memory>
#include <vector>
#include <string>
#include <cmath>

#include <imgui.h>
#include <imgui_internal.h>
#include <tinyfiledialogs.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <App.hpp>
#include <AppContext.hpp>
#include <Camera.hpp>
#include <CubeRenderer.hpp>
#include <CustomReader.hpp>

namespace UserInterface {

    void SetCustomTheme();

    template <typename Callable>
    inline void CreateControlSection(Application::AppContext* appContext, const char* label, Callable&& content) {
        ImGuiStyle& style = ImGui::GetStyle();

        ImGuiID id = ImGui::GetID(label);
        bool isOpen = ImGui::GetStateStorage()->GetBool(id, true);
        ImVec4 baseColor = style.Colors[ImGuiCol_Header];
        ImVec4 activeColor = style.Colors[ImGuiCol_HeaderActive];
        ImGui::PushStyleColor(ImGuiCol_Header, isOpen ? activeColor : baseColor);
        ImGui::PushFont(appContext->fontBold);
        if (ImGui::CollapsingHeader(label, ImGuiTreeNodeFlags_DefaultOpen)) {
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