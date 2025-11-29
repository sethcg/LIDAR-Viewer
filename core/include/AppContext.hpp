#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <vector>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <imgui.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE

#define MINIMUM_WINDOW_WIDTH 640
#define MINIMUM_WINDOW_HEIGHT 360

#define GLSL_VERSION "#version 330"

struct AppContext {
    int width;                      // CURRENT WINDOW WIDTH
    int height;                     // CURRENT WINDOW HEIGHT
    SDL_Window* window;             
    ImDrawData* imgui_data;
    SDL_GLContext opengl_context;   // OPENGL CONTEXT
    GLuint program;                 // SHADER PROGRAM
    GLuint vao;                     // VERTEX ARRAY OBJECT
    GLuint vbo;                     // VERTEX BUFFER OBJECT

    const char* filepath;
    std::vector<float> triColor;    // CURRENT TRIANGLE COLOR

    AppContext() {
        width = WINDOW_WIDTH;
        height = WINDOW_HEIGHT;
        window = nullptr;
        imgui_data = nullptr;
        opengl_context = nullptr;
        program = 0;
        vao = 0;
        vbo = 0;
        filepath = "";
        triColor = { 1.0f, 1.0f, 1.0f };
    }
};

#endif