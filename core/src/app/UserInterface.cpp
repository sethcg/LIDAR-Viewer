#include <functional>
#include <string>

#include <imgui.h>
#include <imgui_internal.h>
#include <tinyfiledialogs.h>

#include <AppContext.hpp>
#include <UserInterface.hpp>

namespace UserInterface {

    void SetCustomTheme() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 6.0f;
        style.FrameRounding = 4.0f;
        style.ChildRounding = 6.0f;
        style.PopupRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.ScrollbarRounding = 6.0f;
        style.WindowBorderSize = 0.0f;
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.FramePadding = ImVec2(10.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
        style.IndentSpacing = 20.0f;
        style.FrameBorderSize = 0.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);        // WINDOW TITLE CENTERED
        style.WindowMenuButtonPosition = ImGuiDir_None;     // WINDOW NO COLLAPSE BUTTON
        style.ScrollbarSize = 12.0f;
        style.GrabMinSize = 10.0f;

        ImVec4* colors = style.Colors;
        ImGui::StyleColorsDark();

        colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.22f, 0.40f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.22f, 0.40f, 1.0f);

        colors[ImGuiCol_Button] = ImVec4(0.12f, 0.15f, 0.2f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.35f, 0.40f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.45f, 0.50f, 1.0f);

        colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.15f, 0.2f, 1.0f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.35f, 0.40f, 1.0f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.45f, 0.50f, 1.0f);
    }

    void TrackChange(bool updated, bool* state) {
        if (updated && state) {
            *state = true;
        }
    }

    void FileSelectButton(
        Application::AppContext* appContext,
        const char* text, 
        ImVec2 size,
        const std::function<void(Application::AppContext*, std::string)>& callback)
    {
        if (ImGui::Button(text, size)) {
            const char* filters[] = { "*.las", "*.laz" };
            const char* selected = tinyfd_openFileDialog(
                "Select a file", "",
                2, // NUMBER OF FILTERS
                filters,
                ".LAZ and .LAS files",
                0 // DO NOT ALLOW MULTIPLE SELECTIONS
            );
            if (selected) {
                std::string str_filepath(selected);
                callback(appContext, str_filepath);
            }
        }
    }

    void FileSelectLabel(const char * text, ImVec2 size, ImVec2 padding) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (!window) return;

        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4 bgColor = style.Colors[ImGuiCol_Button];
        ImVec4 textColor = style.Colors[ImGuiCol_Text];

        ImVec2 textPos = ImGui::GetCursorScreenPos();
        ImVec2 textSize = ImGui::CalcTextSize(text);

        ImVec2 bgMax = ImVec2(
            textPos.x + ImGui::GetContentRegionAvail().x,
            textPos.y + MAX(textSize.y, size.y)
        );

        ImGui::GetWindowDrawList()->AddRectFilled(
            textPos, bgMax,
            ImGui::ColorConvertFloat4ToU32(bgColor),
            style.FrameRounding
        );

        ImVec2 clipMin = ImVec2(textPos.x + padding.x, textPos.y);
        ImVec2 clipMax = ImVec2(bgMax.x - padding.x, bgMax.y);

        window->DrawList->PushClipRect(clipMin, clipMax, true);
        
        ImGui::SetCursorScreenPos(clipMin);
        ImGui::TextColored(textColor, "%s", text);

        window->DrawList->PopClipRect();
    }

}