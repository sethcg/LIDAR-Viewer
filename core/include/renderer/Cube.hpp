#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Data {

    struct Cube {
        glm::vec3 position;
        glm::vec3 color;
        uint16_t intensity;
        float scale;
        
        Cube(const glm::vec3& pos, glm::vec3 col) {
            position = pos;
            color = col;
            scale = 1.0f;
        }

        Cube(const glm::vec3& position, uint16_t intensity) {
            this->position = position;
            this->intensity = intensity;
            color = glm::vec3(1.0f);
            scale = 1.0f;
        }
    };

}