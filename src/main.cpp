// #include <pdal/pdal.hpp>
// #include <pdal/PipelineManager.hpp>
// #include <pdal/PointView.hpp>
// #include <iostream>
// #include <fstream>
// #include <sstream>

// #include <CustomReader.hpp>

// int main(int argc, char* argv[]) {

//     std::string input = "../assets/autzen.laz";
//     std::string output = "../assets/output.txt";

//     return CustomReader::ReadWritePointData(input, output);
// }

#define SDL_MAIN_USE_CALLBACKS 1 // USE CALLBACKS INSTEAD OF THE "main()" FUNCTION

#include <fstream>
#include <sstream>
#include <string>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include <AppContext.hpp>
#include <UserInterface.hpp>

std::string LoadTextFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Failed to load shader file: %s\n", path);
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void CheckShader(GLuint shader, const char* name) {
    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Shader compile error (%s): %s\n", name, log);
    }
}

void CheckProgram(GLuint program) {
    GLint ok;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(program, 1024, nullptr, log);
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Program link error: %s\n", log);
    }
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // SETUP APP STATE
    AppContext* appContext = new AppContext();
    if (appContext == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    *appstate = appContext;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    appContext->window = SDL_CreateWindow("Example", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS);
    appContext->opengl_context = SDL_GL_CreateContext(appContext->window);
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "%s", "Failed to load GLAD\n");
        return SDL_APP_FAILURE;
    }

    // OPENGL TRIANGLE SETUP
    std::string vertSrc = LoadTextFile("../assets/shaders/triangle.vert");
    std::string fragSrc = LoadTextFile("../assets/shaders/triangle.frag");

    const char* vsrc = vertSrc.c_str();
    const char* fsrc = fragSrc.c_str();

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vsrc, nullptr);
    glCompileShader(vert);
    CheckShader(vert, "vertex");

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fsrc, nullptr);
    glCompileShader(frag);
    CheckShader(frag, "fragment");

    appContext->program = glCreateProgram();
    glAttachShader(appContext->program, vert);
    glAttachShader(appContext->program, frag);
    glLinkProgram(appContext->program);
    CheckProgram(appContext->program);

    glDeleteShader(vert);
    glDeleteShader(frag);

    // TRIANGLE GEOMETRY
    float triangle[] = {
        -0.5f, -0.5f, 0.f,
         0.5f, -0.5f, 0.f,
         0.0f,  0.5f, 0.f
    };

    glGenVertexArrays(1, &appContext->vao);
    glGenBuffers(1, &appContext->vbo);

    glBindVertexArray(appContext->vao);

    glBindBuffer(GL_ARRAY_BUFFER, appContext->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

    // SETUP IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // SETUP IMGUI WINDOW/COLOR THEME
    UserInterface::SetCustomTheme();

    // INITIALIZE BACKENDS
    ImGui_ImplSDL3_InitForOpenGL(appContext->window, appContext->opengl_context);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    AppContext* appContext = (AppContext*) appstate;

    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_WINDOW_RESIZED:
            SDL_GetWindowSize(appContext->window, &appContext->width, &appContext->height);
    }

    ImGui_ImplSDL3_ProcessEvent(event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    AppContext* appContext = (AppContext*) appstate;
    
    // INITIALIZE IMGUI FRAME
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    
    ImGui::Begin("Control Panel");

    UserInterface::FileSelectButton(appContext, "Select File", ImVec2(0, 28));
    ImGui::SameLine(0.0f, 10.0f);
    UserInterface::FileSelectLabel(appContext->filepath, ImVec2(0, 28));

    ImGui::ColorEdit3("Color", appContext->triColor.data());

    ImGui::End();
    ImGui::Render();

    glViewport(0, 0, appContext->width, appContext->height);
    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(appContext->program);
    GLint loc = glGetUniformLocation(appContext->program, "uColor");
    glUniform3fv(loc, 1, appContext->triColor.data());

    glBindVertexArray(appContext->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // RENDER IMGUI
    appContext->imgui_data = ImGui::GetDrawData();
    ImGui_ImplOpenGL3_RenderDrawData(appContext->imgui_data);

    SDL_GL_SwapWindow(appContext->window);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    AppContext* appContext = (AppContext*) appstate;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(appContext->program);
    glDeleteBuffers(1, &appContext->vbo);
    glDeleteVertexArrays(1, &appContext->vao);

    SDL_GL_DestroyContext(appContext->opengl_context);
    SDL_DestroyWindow(appContext->window);
    SDL_Quit();
}