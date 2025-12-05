#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <AppContext.hpp>

namespace TextRenderer {

    void Init(TTF_Font* textFont);

    void UpdateFPS();

    void Render(Application::AppContext* appContext);
    
    void Shutdown();

}