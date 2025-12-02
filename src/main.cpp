#define SDL_MAIN_USE_CALLBACKS 1 // USE CALLBACKS INSTEAD OF THE "main()" FUNCTION

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
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

#include <chrono>

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

    // SETUP OPENGL (WITH GLAD)
    if(CreateGLContext(appContext)) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
        return SDL_APP_FAILURE;
    };

    // INITIALIZE RENDERER
    glEnable(GL_DEPTH_TEST);

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<CustomReader::Point> points;
    CustomReader::GetPointData("../assets/lake.laz", &points, 1);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "NUMBER OF POINTS FOR \"lake.laz\": %llu", points.size());
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "DURATION FOR \"lake.laz\": %ld\n\n", duration.count());

    CubeRenderer::Init();
    for (size_t i = 0; i < points.size(); ++i) {
        const auto& point = points[i];
        CubeRenderer::Add(CubeRenderer::Cube(glm::vec3(point.x, point.y, point.z)));
        // SDL_LogDebug(
        //     SDL_LOG_CATEGORY_APPLICATION,
        //     "INDEX: %lu, X: %.10f, Y: %.10f, Z: %.10f",
        //     static_cast<unsigned long>(i), point.x, point.y, point.z
        // );
    }

    // INITIALIZE CAMERA ONCE WHEN ALL CUBES ADDED
    CubeRenderer::InitCamera(appContext);
    
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
    UserInterface::FileSelectButton(appContext, "Select File", ImVec2(0, 28));
    ImGui::SameLine(0.0f, 10.0f);
    UserInterface::FileSelectLabel(appContext->filepath, ImVec2(0, 28));

    // CUBE OPTIONS
    ImGui::ColorEdit3("Global Color", glm::value_ptr(appContext->globalColor));
    ImGui::SliderFloat("Global Scale", &appContext->globalScale, 0.05f, 1.0f);

    ImGui::End();
    ImGui::Render();
    
    CubeRenderer::Render(appContext);

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

    SDL_GL_DestroyContext(appContext->opengl_context);
    SDL_DestroyWindow(appContext->window);
    SDL_Quit();
}