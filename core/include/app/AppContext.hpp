#pragma once

#include <atomic>
#include <memory>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CubeRenderer.hpp>
#include <FreeCamera.hpp>
#include <OrbitalCamera.hpp>
#include <TextRenderer.hpp>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE

#define MINIMUM_WINDOW_WIDTH 640
#define MINIMUM_WINDOW_HEIGHT 360

#define GLSL_VERSION "#version 330"

namespace Application {

    struct AppContext {
        std::string filepath;
        
        float globalScale;

        ImFont* fontBold;
        ImFont* fontRegular;

        Camera* activeCamera = nullptr;
        std::unique_ptr<FreeCamera> freeCamera;
        std::unique_ptr<OrbitalCamera> orbitalCamera;

        std::unique_ptr<CubeRenderer> cubeRenderer;
        std::unique_ptr<TextRenderer> textRenderer;

        // MULTI-THREAD FLAGS FOR READING POINT DATA
        std::atomic<bool> isReadingFlag { false };
        std::atomic<bool> doneReadingFlag { false };

        AppContext() {
            filepath = "";
            globalScale = 0.05f;
        }
        
        inline void SwitchCamera(SDL_Window *window, int width, int height) {
            if (activeCamera == freeCamera.get()) {
                SDL_SetWindowRelativeMouseMode(window, false);
                orbitalCamera->Resize(width, height);
                activeCamera = orbitalCamera.get();
            } else if(!filepath.empty()) {
                SDL_SetWindowRelativeMouseMode(window, true);
                freeCamera->Resize(width, height);
                activeCamera = freeCamera.get();
            }
        }

    };

}
