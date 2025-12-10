#pragma once

#include <memory>
#include <vector>
#include <string>

#include <Point.hpp>
#include <Camera.hpp>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE

#define MINIMUM_WINDOW_WIDTH 640
#define MINIMUM_WINDOW_HEIGHT 360

#define GLSL_VERSION "#version 330"

namespace Application {

    struct AppContext {
        bool enableCulling;
        std::string filepath;
        std::unique_ptr<std::vector<Data::Point>> points;
        
        std::unique_ptr<Camera> camera;

        AppContext() {
            enableCulling = true;
            filepath = "";
            points = std::make_unique<std::vector<Data::Point>>();
        }
    };

}
