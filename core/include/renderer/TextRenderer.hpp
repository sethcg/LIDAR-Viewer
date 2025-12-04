#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <AppContext.hpp>

namespace TextRenderer {

    extern GLuint shaderProgram;
    extern GLuint vao;
    extern GLuint vbo;

    extern GLuint textTexture;
    extern int textureWidth;
    extern int textureHeight;

    extern GLint uProjLocation;

    void Init(Application::AppContext* appContext);

    void UpdateFPS(Application::AppContext* appContext);

    void Render(Application::AppContext* appContext);
    
    void Shutdown(Application::AppContext* appContext);

}