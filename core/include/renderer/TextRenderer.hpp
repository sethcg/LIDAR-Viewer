#pragma once

#include <string>
#include <cstdio>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <RendererHelper.hpp>

namespace TextRenderer {

    void Init(TTF_Font* textFont);

    void UpdateFPS();

    void Render(int width, int height);
    
    void Shutdown();

}