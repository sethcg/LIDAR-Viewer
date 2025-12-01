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

    // INITIALIZE OPENGL RESOURCES
    void Init();

    // RENDER CUBES WITH THE PROVIDED ROTATION AND ANGLE
    void Render(Application::AppContext* appContext);

    void Add(const Cube& cube);
    
    void Clear();
}

#endif