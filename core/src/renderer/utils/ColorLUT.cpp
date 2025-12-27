#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <ColorLUT.hpp>
#include <ColorRamp.hpp>

namespace Renderer::Utils {
        
    void ColorLUT::Init(Data::ColorRampType type, size_t size) {
        lutSize = size;
        lut.resize(lutSize);

        if (!lutTexture) glGenTextures(1, &lutTexture);

        BuildLUT(type);
    }

    void ColorLUT::Shutdown() {
        if (lutTexture) {
            glDeleteTextures(1, &lutTexture);
            lutTexture = 0;
        }
    }

    void ColorLUT::Update(Data::ColorRampType type) {
        BuildLUT(type);
    }

    void ColorLUT::BuildLUT(Data::ColorRampType type) {
        std::vector<glm::vec3> colorRamp = Data::ColorRamp::GetColorRamp(type);
        lut.resize(lutSize);

        for (size_t i = 0; i < lutSize; ++i) {
            float t = float(i) / float(lutSize - 1);
            lut[i] = Data::ColorMap(t, colorRamp);
        }

        if (!lutTexture) glGenTextures(1, &lutTexture);
        
        glBindTexture(GL_TEXTURE_1D, lutTexture);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, static_cast<GLsizei>(lutSize), 0, GL_RGB, GL_FLOAT, lut.data());
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    }

    void ColorLUT::Bind(GLuint textureUnit) const {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_1D, lutTexture);
    }

}