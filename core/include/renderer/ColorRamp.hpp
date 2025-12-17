#pragma once

#include <memory>
#include <optional>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

namespace Data {

    const static inline std::vector<glm::vec3> GenerateColorRamp(const std::vector<glm::vec3>& colors, size_t totalSteps) {
        std::vector<glm::vec3> colorRamp(totalSteps);
        
        size_t numTransitions = colors.size() - 1;
        size_t stepsPerTransition = totalSteps / numTransitions;
        size_t remainder = totalSteps % numTransitions;

        size_t colorIndex = 0;
        for (size_t index = 0; index < numTransitions; ++index) {
            glm::vec3 startColor = colors[index];
            glm::vec3 endColor = colors[index + 1];

            float stepSize = float(stepsPerTransition + (index < remainder ? 1 : 0));
            glm::vec3 step = (endColor - startColor) / stepSize;

            for (size_t stepIndex = 0; stepIndex < stepSize; ++stepIndex) {
                colorRamp[colorIndex++] = startColor + step * float(stepIndex);
            }
        }
        return colorRamp;
    }

    enum class ColorRampType {
        HeatMap,
        Rainbow,
        Pastel,
        BlueRed,
        Grayscale
    };

    static const char* ColorRampNames[] = {
        "HeatMap",
        "Rainbow",
        "Pastel",
        "BlueRed",
        "Grayscale"
    };

    struct ColorRamp {

        const static inline std::vector<glm::vec3> HeatMap = {
            glm::vec3(0.0f, 0.0f, 0.5f),    // DARK BLUE (COLD)
            glm::vec3(0.0f, 0.0f, 1.0f),    // BLUE
            glm::vec3(0.0f, 1.0f, 1.0f),    // CYAN
            glm::vec3(1.0f, 1.0f, 0.0f),    // YELLOW
            glm::vec3(1.0f, 0.0f, 0.0f)     // RED (HOT)
        };
        
        const static inline std::vector<glm::vec3> Rainbow = {
            glm::vec3(0.8f, 0.0f, 1.0f),    // BRIGHT VIOLET
            glm::vec3(0.0f, 0.4f, 1.0f),    // BRIGHT BLUE
            glm::vec3(0.0f, 0.7f, 1.0f),    // BRIGHT CYAN
            glm::vec3(0.3f, 1.0f, 0.3f),    // BRIGHT GREEN
            glm::vec3(1.0f, 1.0f, 0.3f),    // BRIGHT YELLOW
            glm::vec3(1.0f, 0.7f, 0.2f),    // BRIGHT ORANGE
            glm::vec3(1.0f, 0.0f, 0.0f)     // BRIGHT RED
        };

        const static inline std::vector<glm::vec3> Pastel = {
            glm::vec3(0.4f, 0.6f, 0.8f),  // LIGHT BLUE
            glm::vec3(0.6f, 0.4f, 0.8f),  // LIGHT PURPLE
            glm::vec3(0.5f, 0.7f, 0.5f),  // LIGHT GREEN
            glm::vec3(0.8f, 0.8f, 0.5f),  // LIGHT YELLOW
            glm::vec3(0.8f, 0.6f, 0.5f),  // LIGHT ORANGE
            glm::vec3(0.8f, 0.5f, 0.7f)   // LIGHT PINK
        };

        // LAZY INITIALIZING
        static inline std::optional<std::vector<glm::vec3>> BlueRed = std::nullopt;
        static std::vector<glm::vec3>& GetBlueRedColorRamp() {
            if (!BlueRed) {
                BlueRed = GenerateColorRamp(
                    { 
                        glm::vec3(0.0f, 0.0f, 1.0f),
                        glm::vec3(1.0f, 0.0f, 0.0f)
                    }, 20
                );
            }
            return *BlueRed;
        }

        // LAZY INITIALIZING
        static inline std::optional<std::vector<glm::vec3>> Grayscale = std::nullopt;
        static std::vector<glm::vec3>& GetGrayscaleColorRamp() {
            if (!Grayscale) {
                Grayscale = GenerateColorRamp(
                    { 
                        glm::vec3(0.0f, 0.0f, 0.0f),
                        glm::vec3(1.0f, 1.0f, 1.0f)
                    }, 8
                );
            }
            return *Grayscale;
        }

        static std::vector<glm::vec3>& GetColorRamp(ColorRampType type) {
            switch (type) {
                case ColorRampType::HeatMap:
                    return const_cast<std::vector<glm::vec3>&>(HeatMap);
                case ColorRampType::Rainbow:
                    return const_cast<std::vector<glm::vec3>&>(Rainbow);
                case ColorRampType::Pastel:
                    return const_cast<std::vector<glm::vec3>&>(Pastel);
                case ColorRampType::BlueRed:
                    return Data::ColorRamp::GetBlueRedColorRamp();
                case ColorRampType::Grayscale:
                    return Data::ColorRamp::GetGrayscaleColorRamp();
                default: 
                    return const_cast<std::vector<glm::vec3>&>(HeatMap);
            }
        }

    };

    const static inline glm::vec3 ColorMap(float normalizedValue, const std::vector<glm::vec3>& colorRamp) {
        normalizedValue = glm::clamp(normalizedValue, 0.0f, 1.0f);
        
        // CALCULATE POSITION IN THE COLOR RAMP BASED ON NORMALIZED VALUE
        float factor = normalizedValue * (colorRamp.size() - 1);
        size_t index = static_cast<size_t>(factor);
        float blendFactor = factor - index;

        // GET THE TWO NEAREST COLORS TO INTERPOLATE BETWEEN
        glm::vec3 colorStart = colorRamp[index];
        glm::vec3 colorEnd = (index + 1 < colorRamp.size()) ? colorRamp[index + 1] : colorStart;

        // INTERPOLATE COLOR VALUE
        return glm::mix(colorStart, colorEnd, blendFactor);
    }
    
}