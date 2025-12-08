#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <App.hpp>

using namespace Application;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    *appstate = &App::Instance();
    return App::Instance().Init(argc, argv);
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    return App::Instance().ProcessEvent(event);
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    return App::Instance().Frame();
}

void SDL_AppQuit(void* appstate, SDL_AppResult res) {
    App::Instance().Shutdown();
}
