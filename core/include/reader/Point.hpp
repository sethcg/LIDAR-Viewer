#ifndef POINT_DATA_H
#define POINT_DATA_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

#include <Point.hpp>

namespace Data {

    struct Point {
        double x;               // X COORDINATE
        double y;               // Y COORDINATE
        double z;               // Z COORDINATE
        uint16_t intensity;
        float normalized;       // NORMALIZED INTENSITY VALUE [0-1]
        double r;               // RED
        double g;               // GREEN
        double b;               // BLUE
    };

    glm::vec3 ColorMap(float normalizedValue);
    
}

#endif