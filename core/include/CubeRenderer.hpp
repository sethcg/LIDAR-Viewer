#ifndef CUBE_RENDERER_H
#define CUBE_RENDERER_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <AppContext.hpp>

namespace CubeRenderer {

    struct Cube {
        glm::vec3 position;
        glm::vec3 color;

        float scale;
        
        Cube(const glm::vec3& pos, const glm::vec3& col = glm::vec3(1.0f)) {
            position = pos;
            color = col;
            scale = 1.0f;
        }
    };

    const float vertices[] = {
        // BACK FACE (RED)
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,

        // FRONT FACE (GREEN)
        -0.5f,-0.5f, 0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,

        // LEFT FACE (BLUE)
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        // RIGHT FACE (YELLOW)
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        // BOTTOM FACE (MAGENTA)
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,

        // TOP FACE (CYAN)
        -0.5f, 0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
    };

    // INITIALIZE OPENGL RESOURCES
    void Init();

    void InitCamera(Application::AppContext* appContext);

    // RENDER CUBES WITH THE PROVIDED ROTATION AND ANGLE
    void Render(Application::AppContext* appContext);

    void Add(const Cube& cube);
    
    void Clear();
}

#endif