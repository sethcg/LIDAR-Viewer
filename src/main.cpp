#define SDL_MAIN_USE_CALLBACKS 1 // USE CALLBACKS INSTEAD OF THE "main()" FUNCTION

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include <AppContext.hpp>
#include <UserInterface.hpp>
#include <CubeRenderer.hpp>
#include <CustomReader.hpp>
#include <Point.hpp>
#include <TextRenderer.hpp>

using namespace Application;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);

    // SETUP APP STATE
    AppContext* appContext = new AppContext();
    if (appContext == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    *appstate = appContext;

    // SETUP WINDOW
    if(CreateWindow(appContext, "LIDAR Viewer")) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // SETUP OPENGL WITH THE OPTION TO DISABLE VSYNC (FPS LIMIT)
    if(CreateGLContext(appContext, false)) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
        return SDL_APP_FAILURE;
    };

    // INITIALIZE FONT
    if (!TTF_Init()) SDL_Log("ERROR INITIALIZING SDL_TTF: %s", SDL_GetError());
    appContext->textFont = TTF_OpenFont("../assets/fonts/Roboto-Regular.ttf", 18.0f);
    if (!appContext->textFont) {
        SDL_Log("TTF_OPENFONT FAILED: %s", SDL_GetError());
    }

    CubeRenderer::Init(appContext);
    TextRenderer::Init(appContext);
    
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
    
    ImVec2 minSize(MINIMUM_WINDOW_WIDTH / 2, MINIMUM_WINDOW_HEIGHT / 2);
    ImVec2 maxSize(FLT_MAX, FLT_MAX);
    ImGui::SetNextWindowSize(minSize, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
    ImGui::Begin("Control Panel");

    // FILE SELECTION
    UserInterface::FileSelectButton(appContext, "Select File", ImVec2(0, 28),
        [](AppContext* appContext, std::string filepath){
            appContext->filepath = filepath;
            
            // READ LAZ FILE DATA
            const uint32_t decimationStep = 1;
            CustomReader::GetPointData(
                appContext->filepath,
                &appContext->points,
                decimationStep
            );
            
            // ADD CUBE FOR EACH POINT
            CubeRenderer::Clear();
            for (size_t i = 0; i < appContext->points.size(); ++i) {
                const Data::Point& point = appContext->points[i];
                CubeRenderer::Add(CubeRenderer::Cube(
                    glm::vec3(point.x, point.y, point.z), 
                    Data::ColorMap(point.normalized)
                ));
            }
            CubeRenderer::InitCamera(appContext);
            appContext->globalState->changed = true;
        });
    ImGui::SameLine(0.0f, 10.0f);
    UserInterface::FileSelectLabel(appContext->filepath.c_str(), ImVec2(0, 28));

    // GLOBAL COLOR SELECTION
    UserInterface::TrackChange(
        ImGui::ColorEdit3("Global Color", glm::value_ptr(appContext->globalState->color)),
        &appContext->globalState->changed
    );

    // GLOBAL SCALE SLIDER
    UserInterface::TrackChange(
        ImGui::SliderFloat("Global Scale", &appContext->globalState->scale, 0.05f, 1.0f),
        &appContext->globalState->changed
    );

    ImGui::End();
    ImGui::Render();
    
    CubeRenderer::UpdateInstanceBuffers(appContext);
    CubeRenderer::Render(appContext);

    // UPDATE/RENDER "FRAMES PER SECOND"
    TextRenderer::UpdateFPS(appContext);
    TextRenderer::Render(appContext);

    // RENDER IMGUI
    appContext->imgui_data = ImGui::GetDrawData();
    ImGui_ImplOpenGL3_RenderDrawData(appContext->imgui_data);

    SDL_GL_SwapWindow(appContext->window);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    AppContext* appContext = (AppContext*) appstate;

    TextRenderer::Shutdown(appContext);
    CubeRenderer::Shutdown(appContext);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(appContext->opengl_context);
    SDL_DestroyWindow(appContext->window);
    TTF_Quit();
    SDL_Quit();
}