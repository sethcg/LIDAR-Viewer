#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <vector>
#include <string>

#include <SDL3/SDL.h>
#include <imgui.h>

#include <Point.hpp>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE

#define MINIMUM_WINDOW_WIDTH 640
#define MINIMUM_WINDOW_HEIGHT 360

#define GLSL_VERSION "#version 330"

namespace Application {

    struct AppContext {
        SDL_Window* window;             
        ImDrawData* imgui_data;
        SDL_GLContext opengl_context;   // OPENGL CONTEXT
        
        std::vector<Data::Point> points;

        std::string filepath;

        AppContext() {
            window = nullptr;
            imgui_data = nullptr;     
            opengl_context = nullptr;

            filepath = "";
        }
    };

    SDL_AppResult CreateWindow(AppContext* appContext, const char* title);

    SDL_AppResult CreateGLContext(AppContext* appContext, bool enableVsync);

    // ACCESSOR METHODS
    int& GetWindowWidth();
    int& GetWindowHeight();

}

#endif