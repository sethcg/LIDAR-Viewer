#include <SDL3/SDL.h>
#include <glad/glad.h>

#include <AppContext.hpp>

namespace Application {

    static int windowWidth = WINDOW_WIDTH;
    static int windowHeight = WINDOW_HEIGHT;

    SDL_AppResult CreateWindow(AppContext* appContext, const char* title) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        appContext->window = SDL_CreateWindow(title, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FLAGS); 
        if (!appContext->window) {
            SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }
        return SDL_APP_CONTINUE;
    }

    SDL_AppResult CreateGLContext(AppContext* appContext, bool enableVsync) {
        appContext->opengl_context = SDL_GL_CreateContext(appContext->window);
        SDL_GL_SetSwapInterval((enableVsync ? 1 : 0));
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "%s", "Failed to load GLAD\n");
            return SDL_APP_FAILURE;
        }
        return SDL_APP_CONTINUE;
    }

    // ACCESSOR METHODS
    int& GetWindowWidth() { return windowWidth; };
    int& GetWindowHeight() { return windowHeight; };

}