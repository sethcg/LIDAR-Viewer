#include <string>
#include <cstdio>

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <RendererHelper.hpp>
#include <TextRenderer.hpp>

void TextRenderer::Init(TTF_Font* font) {
    textFont = font;
    textShader = Renderer::CreateShaderProgramFromFiles(
        "../assets/shaders/text/text.vert",
        "../assets/shaders/text/text.frag"
    );

    glUseProgram(textShader);
    uProjLocation = glGetUniformLocation(textShader, "uProj");
    glUniform1i(glGetUniformLocation(textShader, "uTex"), 0);
    glUseProgram(0);

    const float quad[] = {
        0, 0, 0, 0,
        1, 0, 1, 0,
        1, 1, 1, 1,
        0, 1, 0, 1
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    // TEXTURE
    glGenTextures(1, &frameRate.texture);
    glBindTexture(GL_TEXTURE_2D, frameRate.texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void TextRenderer::UpdateFPS() {
    frameRate.frameCount++;

    Uint64 currentTime = SDL_GetTicks();
    if (currentTime - frameRate.lastTime < 500) return;

    frameRate.fps = (float)frameRate.frameCount * 1000.0f / (currentTime - frameRate.lastTime);
    frameRate.lastTime = currentTime;
    frameRate.frameCount = 0;

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "FPS: %.1f", frameRate.fps);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(textFont, buffer, 0, white);
    if (!surface) return;

    SDL_Surface* convertedSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);

    frameRate.texWidth = convertedSurface->w;
    frameRate.texHeight = convertedSurface->h;

    glBindTexture(GL_TEXTURE_2D, frameRate.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, convertedSurface->w, convertedSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, convertedSurface->pixels);

    SDL_DestroySurface(convertedSurface);
}

void TextRenderer::Render(int windowWidth, int windowHeight) {
    if (frameRate.texWidth == 0 || frameRate.texHeight == 0) return;

    glUseProgram(textShader);
    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frameRate.texture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 proj = glm::ortho(0.0f, (float) windowWidth, (float) windowHeight, 0.0f);

    float x = (float) windowWidth - frameRate.texWidth - 10;
    float y = 10.0f;

    glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(x, y, 0.f));
    model = glm::scale(model, glm::vec3((float)frameRate.texWidth, (float)frameRate.texHeight, 1.f));

    glm::mat4 mvp = proj * model;
    glUniformMatrix4fv(uProjLocation, 1, GL_FALSE, glm::value_ptr(mvp));

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_BLEND);
}

void TextRenderer::Shutdown() {
    if (textShader) glDeleteProgram(textShader);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (frameRate.texture) glDeleteTextures(1, &frameRate.texture);

    frameRate = {};
    textShader = vao = vbo = 0;
}