#pragma once

#include <memory>
#include <vector>
#include <string>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>

#include <AppContext.hpp>

namespace Application {

    class App {
        public:
            AppContext appContext;

            SDL_AppResult Init(int argc, char** argv);
            SDL_AppResult ProcessEvent(SDL_Event* event);
            SDL_AppResult Frame();
            void Shutdown();
            
            // SETUP APP INSTANCE (SINGLETON)
            static App& Instance() {
                static App instance;
                return instance;
            }

            // PREVENT COPYING/DELETING OPERATIONS (SINGLETON)
            App(const App&) = delete;
            App(App&&) = delete;
            App& operator=(const App&) = delete;
            App& operator=(App&&) = delete;

        private:
            // PREVENT CONSTRUCT/DESTRUCTION (SINGLETON)
            App() = default;
            ~App() = default;

            bool CreateSDLWindow(const char* title);
            bool CreateGLContext(bool enableVsync);
            void RenderScene(float deltaTime);

            SDL_Window* window = nullptr;
            SDL_GLContext glContext = nullptr;

            int width = WINDOW_WIDTH;
            int height = WINDOW_HEIGHT;

            float deltaTime = 0.0f;
            uint64_t lastTime = 0;
    };

}
