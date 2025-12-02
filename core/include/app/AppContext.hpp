#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <vector>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE

#define MINIMUM_WINDOW_WIDTH 640
#define MINIMUM_WINDOW_HEIGHT 360

#define GLSL_VERSION "#version 330"

namespace Application {

    struct AppContext {
        int width;                      // CURRENT WINDOW WIDTH
        int height;                     // CURRENT WINDOW HEIGHT
        SDL_Window* window;             
        ImDrawData* imgui_data;
        SDL_GLContext opengl_context;   // OPENGL CONTEXT

        glm::vec3 globalColor;
        float globalScale;

        const char* filepath;

        AppContext() {
            width = WINDOW_WIDTH;
            height = WINDOW_HEIGHT;
            window = nullptr;
            imgui_data = nullptr;
            
            opengl_context = nullptr;

            globalColor = glm::vec3(1.0f, 0.0f, 0.0f);
            globalScale = 0.5f;

            filepath = "";
        }
    };

    SDL_AppResult CreateWindow(AppContext* appContext, const char* title);

    SDL_AppResult CreateGLContext(AppContext* appContext);

}

#endif