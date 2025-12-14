#pragma once

#include <memory>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Camera.hpp>
#include <CubeRenderer.hpp>

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
        glm::vec3 globalColor;

        std::unique_ptr<Camera> camera;
        std::unique_ptr<CubeRenderer> cubeRenderer;

        AppContext() {
            filepath = "";
            
            globalScale = 0.05f;
            globalColor = glm::vec3(1.0f);
        }
    };

}
