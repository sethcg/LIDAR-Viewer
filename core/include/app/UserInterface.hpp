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

    void RenderMainPanel(Application::AppContext* appContext);

    void DrawCubeSettings();

    void DrawCameraSettings(Application::AppContext* appContext);

    void DrawFileSelection(Application::AppContext* appContext);

    void FileSelectLabel(const char* text, ImVec2 size, ImVec2 padding);

}