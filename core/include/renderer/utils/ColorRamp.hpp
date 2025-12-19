#pragma once

#include <memory>
#include <optional>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

namespace Data {

    enum class ColorRampType {
        HeatMap,
        Rainbow,
        Pastel,
        BlueRed,
        Grayscale
    };

    static inline const char* ColorRampNames[5] = {
        "Heat Map",
        "Rainbow",
        "Pastel",
        "Blue Red",
        "Grayscale"
    };

    inline std::vector<glm::vec3> GenerateColorRamp(const std::vector<glm::vec3>& colors, size_t totalSteps) {
        std::vector<glm::vec3> colorRamp;
        colorRamp.reserve(totalSteps);
        
        const size_t transitions = colors.size() - 1;
        const size_t baseSteps = totalSteps / transitions;
        const size_t remainder = totalSteps % transitions;

        for (size_t i = 0; i < transitions; ++i) {
            const glm::vec3& start = colors[i];
            const glm::vec3& end = colors[i + 1];

            const size_t steps = baseSteps + (i < remainder ? 1 : 0);
            const glm::vec3 delta = (end - start) / float(steps);

            for (size_t s = 0; s < steps; ++s) {
                colorRamp.emplace_back(start + delta * float(s));
            }
        }
        return colorRamp;
    }

    struct ColorRamp {

        static inline const std::vector<glm::vec3> HeatMap = {
            glm::vec3(0.0f, 0.0f, 0.5f),    // DARK BLUE (COLD)
            glm::vec3(0.0f, 0.0f, 1.0f),    // BLUE
            glm::vec3(0.0f, 1.0f, 1.0f),    // CYAN
            glm::vec3(1.0f, 1.0f, 0.0f),    // YELLOW
            glm::vec3(1.0f, 0.0f, 0.0f)     // RED (HOT)
        };
        
        static inline const std::vector<glm::vec3> Rainbow = {
            glm::vec3(0.8f, 0.0f, 1.0f),    // BRIGHT VIOLET
            glm::vec3(0.0f, 0.4f, 1.0f),    // BRIGHT BLUE
            glm::vec3(0.0f, 0.7f, 1.0f),    // BRIGHT CYAN
            glm::vec3(0.3f, 1.0f, 0.3f),    // BRIGHT GREEN
            glm::vec3(1.0f, 1.0f, 0.3f),    // BRIGHT YELLOW
            glm::vec3(1.0f, 0.7f, 0.2f),    // BRIGHT ORANGE
            glm::vec3(1.0f, 0.0f, 0.0f)     // BRIGHT RED
        };

        static inline const std::vector<glm::vec3> Pastel = {
            glm::vec3(0.4f, 0.6f, 0.8f),    // LIGHT BLUE
            glm::vec3(0.6f, 0.4f, 0.8f),    // LIGHT PURPLE
            glm::vec3(0.5f, 0.7f, 0.5f),    // LIGHT GREEN
            glm::vec3(0.8f, 0.8f, 0.5f),    // LIGHT YELLOW
            glm::vec3(0.8f, 0.6f, 0.5f),    // LIGHT ORANGE
            glm::vec3(0.8f, 0.5f, 0.7f)     // LIGHT PINK
        };

        static const std::vector<glm::vec3>& BlueRed() {
            static const std::vector<glm::vec3> colorRamp =
                GenerateColorRamp({ 
                    glm::vec3(0.0f, 0.0f, 1.0f),    // BLUE
                    glm::vec3(1.0f, 0.0f, 0.0f)     // RED
                }, 20);
            return colorRamp;
        }

        static const std::vector<glm::vec3>& Grayscale() {
            static const std::vector<glm::vec3> colorRamp =
                GenerateColorRamp({ 
                    glm::vec3(0.0f, 0.0f, 0.0f),    // BLACK
                    glm::vec3(1.0f, 1.0f, 1.0f)     // WHITE
                }, 8);
            return colorRamp;
        }

        static const std::vector<glm::vec3>& GetColorRamp(ColorRampType type) {
            switch (type) {
                case ColorRampType::HeatMap:   return HeatMap;
                case ColorRampType::Rainbow:   return Rainbow;
                case ColorRampType::Pastel:    return Pastel;
                case ColorRampType::BlueRed:   return BlueRed();
                case ColorRampType::Grayscale: return Grayscale();
                default:                       return HeatMap;
            }
        }

    };

    inline glm::vec3 ColorMap(float normalizedValue, const std::vector<glm::vec3>& colorRamp) {
        normalizedValue = glm::clamp(normalizedValue, 0.0f, 1.0f);
        
        // CALCULATE POSITION IN THE COLOR RAMP BASED ON NORMALIZED VALUE
        const float scaled = normalizedValue * float(colorRamp.size() - 1);
        const size_t index = std::min(static_cast<size_t>(scaled), colorRamp.size() - 1);
        const float blendFactor = scaled - float(index);

        // GET THE TWO NEAREST COLORS TO INTERPOLATE BETWEEN
        const glm::vec3& colorStart = colorRamp[index];
        const glm::vec3& colorEnd = (index + 1 < colorRamp.size()) ? colorRamp[index + 1] : colorStart;

        // INTERPOLATE COLOR VALUE
        return glm::mix(colorStart, colorEnd, blendFactor);
    }
    
}