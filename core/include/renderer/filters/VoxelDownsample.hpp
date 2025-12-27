#pragma once

#include <algorithm>
#include <chrono>
#include <vector>
#include <string>

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CubeInstance.hpp>
#include <RendererHelper.hpp>

namespace Filters {

    // GPU POINT DATA STRUCT (ALIGNED TO 16 BYTES)
    struct alignas(16) GPUPointData {
        glm::vec3 position;
        float intensity;

        GPUPointData() = default;

        GPUPointData(const glm::vec3& position, float intensity) {
            this->intensity = intensity;
            this->position = position;
        }
    };

    class VoxelDownsample {
        public:
            VoxelDownsample() = default;
            ~VoxelDownsample() { Shutdown(); }

            void Init();
            void Shutdown();

            void Downsample(std::vector<GPUPointData>& output, const std::vector<CubeInstance>& cubes, float voxelSize = 0.0f);

        private:
            void PrepareInput(const std::vector<CubeInstance>& cubes);

            float CalculateVoxelSize(const std::vector<GPUPointData>& input);

        private:
            std::vector<GPUPointData> inputPoints;

            // GPU RESOURCES
            GLuint computeShader = 0;
            GLuint inputPointSSBO = 0;
            GLuint outputPointSSBO = 0;
            GLuint outputCounterSSBO = 0;
            GLuint voxelHashSSBO = 0;

        private:
            // NON-COPYABLE (OWNS GPU RESOURCES)
            VoxelDownsample(const VoxelDownsample&) = delete;
            VoxelDownsample& operator = (const VoxelDownsample&) = delete;
    };

}