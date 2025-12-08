#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Data {

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

}