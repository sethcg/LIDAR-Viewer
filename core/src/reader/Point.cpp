#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

#include <Point.hpp>

namespace Data {

    glm::vec3 ColorMap(float normalizedValue) {
        // BREAK NORMALIZED VALUE INTO SEGMENTS OF COLOR GRADIENTS
        // TODO: IMPROVE/MAKE THIS MORE ACCESSIBLE TO CHANGE WHAT COLOR GRADIENTS SHOW
        if (normalizedValue < 0.25f) {
            return glm::mix(glm::vec3(0, 0, 0.5), glm::vec3(0, 0, 1), normalizedValue / 0.25f);
        }
        if (normalizedValue < 0.5f) {
            return glm::mix(glm::vec3(0, 0, 1), glm::vec3(0, 1, 1), (normalizedValue - 0.25f) / 0.25f);
        }
        if (normalizedValue < 0.75f) {
            return glm::mix(glm::vec3(0, 1, 1), glm::vec3(1, 1, 0), (normalizedValue - 0.5f) / 0.25f);
        }
        return glm::mix(glm::vec3(1, 1, 0), glm::vec3(1, 0, 0), (normalizedValue - 0.75f) / 0.25f);
    }
    
}
