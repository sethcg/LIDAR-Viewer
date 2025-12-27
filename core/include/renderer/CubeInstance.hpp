#pragma once

#include <glm/glm.hpp>

struct CubeInstance {
    glm::vec3 position;
    uint16_t intensity;
    float normalized_intensity = 0.0f;
    
    CubeInstance(const glm::vec3& position, const uint16_t intensity) {
        this->position = position;
        this->intensity = intensity;
    }
};