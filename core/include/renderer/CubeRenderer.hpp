#ifndef CUBE_RENDERER_H
#define CUBE_RENDERER_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <AppContext.hpp>
#include <Camera.hpp>

namespace CubeRenderer {

    struct Cube {
        glm::vec3 position;
        glm::vec3 color;

        float scale;
        
        Cube(const glm::vec3& pos, glm::vec3 col) {
            position = pos;
            color = col;
            scale = 1.0f;
        }
    };

    // TODO: PERFORMANCE IMPROVEMENT TO USE "glm::vec3" AND CUBE INDICES
    // AND ALLOW FOR USING "glDrawElements" INSTEAD OF "glDrawArrays"
    const float vertices[] = {
        // BACK FACE
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,

        // FRONT FACE
        -0.5f,-0.5f, 0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,

        // LEFT FACE
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        // RIGHT FACE
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        // BOTTOM FACE
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,

        // TOP FACE
        -0.5f, 0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
    };

    void Init(Application::AppContext* appContext);

    void UpdateInstanceBuffers(Application::AppContext* appContext);

    void Render(Application::AppContext* appContext);

    void Shutdown(Application::AppContext* appContext);

    void Add(const Cube& cube);
    
    void Clear();

    const std::vector<Cube>& GetCubes();
}

#endif