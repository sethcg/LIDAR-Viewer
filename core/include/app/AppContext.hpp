#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <vector>
#include <string>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui.h>

#include <Point.hpp>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE

#define MINIMUM_WINDOW_WIDTH 640
#define MINIMUM_WINDOW_HEIGHT 360

#define GLSL_VERSION "#version 330"

namespace Application {

    struct FrameRate {
        GLuint fpsTexture;
        int fpsTexW;
        int fpsTexH;
        float fps;
        Uint64 lastFPSTime;
        int frameCount;

        FrameRate() {
            fpsTexture = 0;
            fpsTexW = 0;
            fpsTexH = 0;
            fps = 0.0f;
            lastFPSTime = 0;
            frameCount = 0;
        }
    };

    struct AppContext {
        int width;                      // CURRENT WINDOW WIDTH
        int height;                     // CURRENT WINDOW HEIGHT
        SDL_Window* window;             
        ImDrawData* imgui_data;
        SDL_GLContext opengl_context;   // OPENGL CONTEXT
        
        TTF_Font* textFont;
        FrameRate* frameRate;

        std::vector<Data::Point> points;

        glm::vec3 globalColor;
        float globalScale;

        std::string filepath;

        AppContext() {
            width = WINDOW_WIDTH;
            height = WINDOW_HEIGHT;
            window = nullptr;
            imgui_data = nullptr;     
            opengl_context = nullptr;
            frameRate = new FrameRate();

            globalColor = glm::vec3(1.0f);
            globalScale = 0.5f;

            filepath = "";
        }
    };

    SDL_AppResult CreateWindow(AppContext* appContext, const char* title);

    SDL_AppResult CreateGLContext(AppContext* appContext, bool enableVsync);
}

#endif