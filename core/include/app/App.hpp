#ifndef APP_H
#define APP_H

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
#include <Camera.hpp>
#include <CubeRenderer.hpp>
#include <CustomReader.hpp>
#include <TextRenderer.hpp>
#include <UserInterface.hpp>
#include <Point.hpp>

namespace Application {

    class App {
        public:
            SDL_AppResult Init(int argc, char** argv);
            SDL_AppResult ProcessEvent(SDL_Event* event);
            SDL_AppResult Frame();
            void Shutdown();

        private:
            bool CreateSDLWindow(const char* title);
            bool CreateGLContext(bool enableVsync);
            void RenderScene(float deltaTime);

        public:
            AppContext appContext;

        private:
            SDL_Window* window = nullptr;
            SDL_GLContext glContext = nullptr;

            std::unique_ptr<Camera> camera;

            int width = WINDOW_WIDTH;
            int height = WINDOW_HEIGHT;

            float deltaTime;
            uint64_t lastTime = 0;
    };

}

#endif