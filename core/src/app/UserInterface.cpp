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
#include <Cube.hpp>
#include <CubeRenderer.hpp>
#include <CustomReader.hpp>
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

    void RenderMainPanel(Application::AppContext* appContext) {
        ImVec2 minSize(float(MINIMUM_WINDOW_WIDTH) / 2.0f, float(MINIMUM_WINDOW_HEIGHT) / 2.0f);
        ImVec2 maxSize(FLT_MAX, FLT_MAX);
        ImGui::SetNextWindowSize(minSize, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
        ImGui::Begin("##panel");

        DrawFileSelection(appContext);
        DrawCubeSettings();
        DrawCameraSettings(appContext);

        ImGui::End();
    }

    void DrawCubeSettings() {
        // TITLE
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        ImGui::Text("Cube");
        ImGui::PopStyleColor();

        // GLOBAL COLOR (TINT)
        ImGui::ColorEdit3("Global Color", glm::value_ptr(CubeRenderer::GetGlobalColor()));

        // GLOBAL SCALE
        ImGui::SliderFloat("Global Scale", &CubeRenderer::GetGlobalScale(), 2.0f, 20.0f);

    }

    void DrawCameraSettings(Application::AppContext* appContext) {
        // TITLE
        ImGui::Spacing();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        ImGui::Text("Camera");
        ImGui::PopStyleColor();

        // CAMERA ROTATION SPEED
        ImGui::SliderFloat("Rotation Speed", 
            &appContext->camera->GetRotationSpeed(), 
            0.0f, 
            300.0f
        );

        // CAMERA ZOOM
        ImGui::SliderFloat("Zoom", 
            &appContext->camera->GetTargetZoom(), 
            appContext->camera->GetMinZoom(), 
            appContext->camera->GetMaxZoom());

        // ENABLE CULLING
        ImGui::Checkbox("Enable back-face culling", &appContext->enableCulling);

    }

    void DrawFileSelection(Application::AppContext* appContext) {
        if (ImGui::Button("Select File", ImVec2(0.0f, 28.0f))) {
            const char* filters[] = { "*.las", "*.laz" };
            const char* selected = tinyfd_openFileDialog(
                "Select a file", "",
                2, // NUMBER OF FILTERS
                filters,
                ".LAZ and .LAS files",
                0 // DO NOT ALLOW MULTIPLE SELECTIONS
            );
            if (selected) {
                // READ LAZ FILE DATA
                appContext->filepath = selected;
                CustomReader::GetPointData(appContext->filepath, appContext->points);
                
                // CLEAR, THEN ADD EACH CUBE
                CubeRenderer::Clear();
                CubeRenderer::UpdateBufferSize(appContext->points->size());
                CubeRenderer::AddCubes(*appContext->points);
                appContext->points->clear();
                appContext->camera->RecalculateBounds();
            } else {
                return;
            }
        }
        ImGui::SameLine(0.0f, 10.0f);
        UserInterface::FileSelectLabel(appContext->filepath.c_str(), ImVec2(0.0f, 28.0f), ImVec2(8.0f, 4.0f));
    }

    void FileSelectLabel(const char* text, ImVec2 size, ImVec2 padding) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (!window) return;

        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4 bgColor = style.Colors[ImGuiCol_Button];
        ImVec4 textColor = style.Colors[ImGuiCol_Text];

        ImVec2 textPos = ImGui::GetCursorScreenPos();
        ImVec2 textSize = ImGui::CalcTextSize(text);

        ImVec2 bgMax = ImVec2(
            textPos.x + ImGui::GetContentRegionAvail().x,
            textPos.y + std::fmax(textSize.y, size.y)
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