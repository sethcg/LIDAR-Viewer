#pragma once

#include <algorithm>
#include <chrono>
#include <vector>
#include <string>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CubeInstance.hpp>
#include <RendererHelper.hpp>

namespace Filters {

    class VoxelDownsample {
        public:
            VoxelDownsample();
            ~VoxelDownsample();

            std::vector<CubeInstance> ProcessPoints(std::vector<CubeInstance>& cubes);

        private:
            void CalculateVoxelSize();
            void UpdateBufferSize();

        private:
            float voxelSize = 0.0f;
            uint32_t voxelFlagSize = 0;
            glm::vec3 voxelOrigin = glm::vec3(0.0f);
            glm::vec3 voxelBounds = glm::vec3(0.0f);

            std::vector<glm::vec3> pointData;

            // GPU UNIFORMS
            GLint uVoxelSize = -1;
            GLint uVoxelOrigin = -1;
            GLint uVoxelBounds = -1;
            GLint uVoxelFlagSize = -1;

            // GPU RESOURCES
            GLuint computeProgram = 0;
            GLuint inputPointSSBO = 0;
            GLuint outputFlagSSBO = 0;

        private:
            // NON-COPYABLE (OWNS GPU RESOURCES)
            VoxelDownsample(const VoxelDownsample&) = delete;
            VoxelDownsample& operator = (const VoxelDownsample&) = delete;
    };

}