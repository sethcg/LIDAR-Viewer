#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <ColorRamp.hpp>

namespace Renderer::Utils {

    class ColorLUT {
        public:
            ColorLUT() = default;
            ~ColorLUT() { Shutdown(); }

            void Init(Data::ColorRampType type, size_t lutSize = 256);
            void Shutdown();

            void Update(Data::ColorRampType type);

            void Bind(GLuint textureUnit = 0) const;
            GLuint GetTexture() const { return lutTexture; }

        private:
            void BuildLUT(Data::ColorRampType type);

        private:
            GLuint lutTexture = 0;
            size_t lutSize = 256;

            std::vector<glm::vec3> lut;
    };

}