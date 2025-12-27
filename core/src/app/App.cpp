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
#include <ColorRamp.hpp>
#include <CubeRenderer.hpp>
#include <FreeCamera.hpp>
#include <OrbitalCamera.hpp>
#include <TextRenderer.hpp>
#include <UserInterface.hpp>

namespace Application {

    bool App::CreateSDLWindow(const char* title) {
        // SET SDL GL ATTRIBUTES (OpenGL 4.3 CORE)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        
        SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);

        window = SDL_CreateWindow(title, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS); 
        return window != nullptr;
    }

    bool App::CreateGLContext(bool enableVsync) {
        glContext = SDL_GL_CreateContext(window);
        SDL_GL_SetSwapInterval(enableVsync ? 1 : 0);
        return gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);
    }

    SDL_AppResult App::Init(int argc, char** argv) {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) return SDL_APP_FAILURE;
        if (!CreateSDLWindow("LIDAR Viewer")) return SDL_APP_FAILURE;
        if (!CreateGLContext(false)) return SDL_APP_FAILURE;

        appContext.freeCamera = std::make_unique<FreeCamera>(width, height);
        appContext.orbitalCamera = std::make_unique<OrbitalCamera>(width, height);
        appContext.activeCamera = appContext.orbitalCamera.get();

        appContext.cubeRenderer = std::make_unique<CubeRenderer>();
        appContext.cubeRenderer->Init(Data::ColorRampType::HeatMap);

        TTF_Init();
        TTF_Font* textFont = TTF_OpenFont("../assets/fonts/Roboto-Regular.ttf", 18.0f);
        appContext.textRenderer = std::make_unique<TextRenderer>();
        appContext.textRenderer->Init(textFont);

        // INITIALIZE IMGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext(); 
        ImGuiIO& io = ImGui::GetIO();
        appContext.fontBold = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-Bold.ttf", 16.0f);
        appContext.fontRegular = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-Regular.ttf", 16.0f);
        io.FontDefault = appContext.fontRegular;
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
                glViewport(0, 0, width, height);
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event->key.scancode == SDL_SCANCODE_F) {
                    appContext.SwitchCamera(window, width, height);
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                appContext.activeCamera->ProcessMouseMotion(event->motion.xrel, event->motion.yrel);
                break;
        }

        // DISABLE GUI IN FREE CAMERA MODE
        if(appContext.activeCamera != appContext.freeCamera.get()) {
            ImGui_ImplSDL3_ProcessEvent(event);
        }

        return SDL_APP_CONTINUE;
    }

    void App::RenderScene(float deltaTime) {
        // DRAW BACKGROUND/CLEAR FRAMEBUFFER
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        appContext.activeCamera->ProcessKeyboard(deltaTime);
        appContext.activeCamera->Update(deltaTime);

        appContext.cubeRenderer->Render(
            appContext.activeCamera->GetViewProjection(),
            appContext.globalScale
        );

        // UPDATE THE GPU BUFFERS ONCE WHEN DONE READING DATA
        if (appContext.doneReadingFlag.load(std::memory_order_acquire)) {
            appContext.isReadingFlag.store(false, std::memory_order_release);
            appContext.doneReadingFlag.store(false, std::memory_order_release);

            appContext.cubeRenderer->VoxelDownsample();

            appContext.cubeRenderer->NormalizeIntensities();
            appContext.cubeRenderer->UpdateBuffers();
        }

        appContext.textRenderer->UpdateFPS();
        appContext.textRenderer->Render(width, height);
    }

    SDL_AppResult App::Frame() {
        // UPDATE DELTA TIME
        uint64_t currentTime = SDL_GetPerformanceCounter();
        float deltaTime = float(currentTime - lastTime) / SDL_GetPerformanceFrequency();
        lastTime = currentTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // RENDER GUI
        UserInterface::RenderMainPanel(&appContext);

        // RENDER SCENE (CAMERA, CUBE, TEXT, ...)
        RenderScene(deltaTime);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);

        return SDL_APP_CONTINUE;
    }

    void App::Shutdown() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

}
