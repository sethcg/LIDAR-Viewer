#include <memory>
#include <vector>
#include <string>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include <App.hpp>
#include <AppContext.hpp>
#include <Camera.hpp>
#include <CubeRenderer.hpp>
#include <CustomReader.hpp>
#include <TextRenderer.hpp>
#include <UserInterface.hpp>
#include <Point.hpp>

namespace Application {

    bool App::CreateSDLWindow(const char* title) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);

        window = SDL_CreateWindow(title, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS); 
        if (!window) {
            SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
            return false;
        }
        return true;
    }

    bool App::CreateGLContext(bool enableVsync) {
        glContext = SDL_GL_CreateContext(window);
        SDL_GL_SetSwapInterval((enableVsync ? 1 : 0));
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "%s", "Failed to load GLAD\n");
            return false;
        }
        return true;
    }

    SDL_AppResult App::Init(int argc, char** argv) {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) return SDL_APP_FAILURE;
        if (!CreateSDLWindow("LiDAR Viewer")) return SDL_APP_FAILURE;
        if (!CreateGLContext(false)) return SDL_APP_FAILURE;

        camera = std::make_unique<Camera>(width, height);
        CubeRenderer::Init();

        TTF_Init();
        TTF_Font* textFont = TTF_OpenFont("../assets/fonts/Roboto-Regular.ttf", 18);
        TextRenderer::Init(textFont);
        
        // INITIALIZE IMGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        UserInterface::SetCustomTheme();

        ImGui_ImplSDL3_InitForOpenGL(window, glContext);
        ImGui_ImplOpenGL3_Init(GLSL_VERSION);

        lastTime = SDL_GetPerformanceCounter();
        return SDL_APP_CONTINUE;
    }

    SDL_AppResult App::ProcessEvent(SDL_Event* event) {
        switch (event->type) {
            case SDL_EVENT_QUIT:
                return SDL_APP_SUCCESS;
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                width = event->window.data1;
                height = event->window.data2;
                camera->Resize(width, height);
                break;
        }

        ImGui_ImplSDL3_ProcessEvent(event);
        return SDL_APP_CONTINUE;
    }

    void App::RenderScene(float deltaTime) {
        if (camera) camera->Update(deltaTime);

        CubeRenderer::Render(camera->GetView(), camera->GetProjection());

        TextRenderer::UpdateFPS();
        TextRenderer::Render(width, height);
    }
  
    SDL_AppResult App::Frame() {
        // UPDATE DELTA TIME
        uint64_t currentTime = SDL_GetPerformanceCounter();
        float deltaTime = float(currentTime - lastTime) / SDL_GetPerformanceFrequency();
        lastTime = currentTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // RENDER GUI (IMGUI SETTINGS PANEL)
        UserInterface::RenderMainPanel(camera.get(), &appContext);

        // RENDER SCENE (CAMERA, CUBE, TEXT, ...)
        RenderScene(deltaTime);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);

        return SDL_APP_CONTINUE;
    }

    void App::Shutdown() {
        TextRenderer::Shutdown();
        CubeRenderer::Shutdown();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

}
