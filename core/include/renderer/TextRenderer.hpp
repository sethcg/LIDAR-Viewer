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

class TextRenderer {
public:
    TextRenderer() = default;
    ~TextRenderer() { Shutdown(); }

    void Init(TTF_Font* font);

    void Shutdown();

    void UpdateFPS();

    void Render(int windowWidth, int windowHeight);

    private:
        struct FrameRate {
            GLuint texture = 0;
            int texWidth = 0;
            int texHeight = 0;
            float fps = 0.0f;
            Uint64 lastTime = 0;
            int frameCount = 0;
        };

    private:
        TTF_Font* textFont = nullptr;
        FrameRate frameRate;

        // GPU OBJECTS
        GLuint textShader = 0;
        GLuint vao = 0;
        GLuint vbo = 0;

        // SHADER UNIFORMS
        GLint uProjLocation = -1;

    private:
        // NON-COPYABLE (OWNS GPU RESOURCES)
        TextRenderer(const TextRenderer&) = delete;
        TextRenderer& operator=(const TextRenderer&) = delete;
};
