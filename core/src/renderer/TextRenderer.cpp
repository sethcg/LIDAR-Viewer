#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <TextRenderer.hpp>
#include <RendererHelper.hpp>

namespace TextRenderer {

    static GLuint shaderProgram;
    static GLuint vao;
    static GLuint vbo;

    static GLuint textTexture;
    static int textureWidth = 0;
    static int textureHeight = 0;

    static GLint uProjLocation = -1;

    void Init(Application::AppContext* appContext) {
        std::string vertexSource   = RendererHelper::LoadTextFile("../assets/shaders/text/text.vert");
        std::string fragmentSource = RendererHelper::LoadTextFile("../assets/shaders/text/text.frag");

        GLuint vertexShader   = RendererHelper::CreateShader(vertexSource, GL_VERTEX_SHADER);
        GLuint fragmentShader = RendererHelper::CreateShader(fragmentSource, GL_FRAGMENT_SHADER);

        shaderProgram = RendererHelper::CreateShaderProgram(vertexShader, fragmentShader);

        uProjLocation = glGetUniformLocation(shaderProgram, "uProj");

        float quad[] = {
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
        glGenTextures(1, &textTexture);
        glBindTexture(GL_TEXTURE_2D, textTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "uTex"), 0);
        glUseProgram(0);
    }

    void UpdateFPS(Application::AppContext* appContext) {
        Application::FrameRate* frameRate = appContext->frameRate;
        frameRate->frameCount++;

        Uint64 currentTime = SDL_GetTicks();
        if (currentTime - frameRate->lastFPSTime < 500) return;

        frameRate->fps = (float)frameRate->frameCount * 1000.0f / (currentTime - frameRate->lastFPSTime);
        frameRate->lastFPSTime = currentTime;
        frameRate->frameCount = 0;

        char buffer[64];
        snprintf(buffer, sizeof(buffer), "FPS: %.1f", frameRate->fps);

        SDL_Color white = {255, 255, 255, SDL_ALPHA_OPAQUE};
        SDL_Surface* surface = TTF_RenderText_Blended(appContext->textFont, buffer, 0, white);
        if (!surface) return;

        // FORMAT THE SURFACE
        SDL_Surface* convertedSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(surface);

        textureWidth = convertedSurface->w;
        textureHeight = convertedSurface->h;

        glBindTexture(GL_TEXTURE_2D, textTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, convertedSurface->w, convertedSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, convertedSurface->pixels);

        SDL_DestroySurface(convertedSurface);
    }

    void Render(Application::AppContext* appContext) {
        if (textureWidth == 0 || textureHeight == 0) return;

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textTexture);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glm::mat4 proj = glm::ortho(0.0f, (float) appContext->width, (float) appContext->height, 0.0f);

        float x = (float) appContext->width - textureWidth - 10;
        float y = 10.0f;

        glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(x, y, 0.f));
        model = glm::scale(model, glm::vec3((float)textureWidth, (float)textureHeight, 1.f));

        glm::mat4 mvp = proj * model;
        glUniformMatrix4fv(uProjLocation, 1, GL_FALSE, glm::value_ptr(mvp));

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(0);
        glUseProgram(0);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    void Shutdown(Application::AppContext* appContext) {
        glDeleteTextures(1, &textTexture);
        glDeleteProgram(shaderProgram);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

}
