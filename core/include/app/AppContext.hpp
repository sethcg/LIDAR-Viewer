#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include <memory>
#include <vector>
#include <string>

#include <Point.hpp>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE

#define MINIMUM_WINDOW_WIDTH 640
#define MINIMUM_WINDOW_HEIGHT 360

#define GLSL_VERSION "#version 330"

namespace Application {

    struct AppContext {
        std::string filepath;
        std::unique_ptr<std::vector<Data::Point>> points;

        AppContext() {
            filepath = "";
        }
    };

}

#endif