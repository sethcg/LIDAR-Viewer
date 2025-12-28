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

    class VoxelDownsampleFilter {
        public:
            VoxelDownsampleFilter();
            ~VoxelDownsampleFilter();

            std::vector<CubeInstance> ProcessPoints(std::vector<CubeInstance>& cubes);

        private:
            void CalculateVoxelSize();
            void UpdateBufferSize();

        private:
            float voxelSize = 0.0f;
            uint32_t voxelCount = 0;
            glm::vec3 voxelOrigin = glm::vec3(0.0f);
            glm::vec3 voxelBounds = glm::vec3(0.0f);

            std::vector<glm::vec3> pointData;

            // GPU UNIFORMS
            GLint uVoxelSize = -1;
            GLint uVoxelCount = -1;
            GLint uVoxelOrigin = -1;
            GLint uVoxelBounds = -1;

            // GPU RESOURCES
            GLuint computeProgram = 0;
            GLuint inputPointSSBO = 0;
            GLuint outputFlagSSBO = 0;

        private:
            // NON-COPYABLE (OWNS GPU RESOURCES)
            VoxelDownsampleFilter(const VoxelDownsampleFilter&) = delete;
            VoxelDownsampleFilter& operator = (const VoxelDownsampleFilter&) = delete;
    };

}