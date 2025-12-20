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
#include <LazReader.hpp>
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

    inline void TooltipInfoIcon(const bool isVisible, const char* tooltipText, Application::AppContext* appContext) {
        if(!isVisible) return;

        ImGuiStyle& style = ImGui::GetStyle();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const char* iconText = "i";
        const float iconFontSize = 20.0f;

        float textOffset = iconFontSize - ImGui::GetTextLineHeight();
        ImVec2 textSize = appContext->fontBold->CalcTextSizeA(iconFontSize, FLT_MAX, -1.0f, iconText);
        ImVec2 boxSize = ImVec2(
            textSize.x + (style.FramePadding.x * 1.5f), 
            textSize.y + (style.FramePadding.y * 1.5f)
        );
        ImVec2 min = ImVec2(
            ImGui::GetCursorScreenPos().x,
            ImGui::GetCursorScreenPos().y + textOffset
        );
        ImVec2 max = ImVec2(
            ImGui::GetCursorScreenPos().x + boxSize.x, 
            ImGui::GetCursorScreenPos().y + boxSize.y - textOffset
        );

        // HOVER DETECTION
        ImVec2 mousePos = ImGui::GetIO().MousePos;
        bool hovered = 
            mousePos.x >= min.x && mousePos.x <= max.x &&
            mousePos.y >= min.y && mousePos.y <= max.y;
        ImU32 backgroundColor = ImGui::GetColorU32(hovered
            ? ImVec4(0.20f, 0.45f, 0.80f, 0.30f)
            : ImVec4(0.30f, 0.55f, 0.90f, 0.45f)
        );

        drawList->AddRectFilled(min, max, backgroundColor, 8.0f);
        
        ImVec2 textPos = ImVec2(
            min.x + (style.FramePadding.x * 0.75f),
            min.y + (style.FramePadding.y * 0.75f) - textOffset
        );
        drawList->AddText(appContext->fontBold, iconFontSize, textPos, ImGui::GetColorU32(ImGuiCol_Text), iconText);

        if (hovered) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f);
            ImGui::TextUnformatted(tooltipText);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        ImGui::Dummy(boxSize);
        ImGui::SameLine();
    }

    void DrawFileSelectionSettings(Application::AppContext* appContext);

    void DrawCubeSettings(Application::AppContext* appContext);

    void DrawOrbitalCameraSettings(Application::AppContext* appContext);

    void RenderMainPanel(Application::AppContext* appContext);

}