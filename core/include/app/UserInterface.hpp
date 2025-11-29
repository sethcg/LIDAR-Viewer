#ifndef USER_INTERFACE_HELPER_H
#define USER_INTERFACE_HELPER_H

#include <imgui.h>
#include <AppContext.hpp>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

namespace UserInterface {

    using namespace Application;

    void SetCustomTheme();

    void FileSelectButton(AppContext* appContext, const char* text, ImVec2 size);

    void FileSelectLabel(const char* text, ImVec2 size, ImVec2 padding = ImVec2(8.0f, 4.0f));

}

#endif