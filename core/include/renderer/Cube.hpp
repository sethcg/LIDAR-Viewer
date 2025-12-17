#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

namespace Data {

    struct Cube {
        glm::vec3 position;
        glm::vec3 color;

        uint16_t intensity;
        double normalized_intensity = 0;
        
        Cube(const glm::vec3& position, const glm::vec3& color, const uint16_t intensity) {
            this->position = position;
            this->color = color;
            this->intensity = intensity;
        }
    };

}