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
#include <VoxelDownsample.hpp>

namespace Filters {

    void VoxelDownsample::Init() {
        computeShader = Renderer::CreateComputeShaderProgram("../assets/shaders/compute/voxel_downsample.comp");

        glGenBuffers(1, &inputPointSSBO);
        glGenBuffers(1, &outputPointSSBO);
        glGenBuffers(1, &outputCounterSSBO);
        glGenBuffers(1, &voxelHashSSBO);
    }

    void VoxelDownsample::Shutdown() {
        if (computeShader) glDeleteProgram(computeShader);
        if (inputPointSSBO) glDeleteBuffers(1, &inputPointSSBO);
        if (outputPointSSBO) glDeleteBuffers(1, &outputPointSSBO);
        if (outputCounterSSBO) glDeleteBuffers(1, &outputCounterSSBO);
        if (voxelHashSSBO) glDeleteBuffers(1, &voxelHashSSBO);

        computeShader = 0;
        inputPointSSBO = 0;
        outputPointSSBO = 0;
        outputCounterSSBO = 0;
        voxelHashSSBO = 0;
    }

    void VoxelDownsample::PrepareInput(const std::vector<CubeInstance>& cubes) {
        inputPoints.clear();

        if(cubes.empty()) return;

        inputPoints.reserve(cubes.size());
        for (const auto& cube : cubes) {
            inputPoints.emplace_back(cube.position, static_cast<float>(cube.intensity));
        }
    }

    float VoxelDownsample::CalculateVoxelSize(const std::vector<GPUPointData>& input) {
        if (input.empty()) return 1.0f;

        glm::vec3 minPoint = input[0].position;
        glm::vec3 maxPoint = input[0].position;

        for (const auto& point : input) {
            minPoint = glm::min(minPoint, point.position);
            maxPoint = glm::max(maxPoint, point.position);
        }

        float boundsX = maxPoint.x - minPoint.x;
        float boundsY = maxPoint.y - minPoint.y;

        float area = std::max(boundsX * boundsY, 1.0f);
        float density = static_cast<float>(input.size()) / area;
        float spacing = std::sqrt(1.0f / density);

        float voxelSize = spacing * 1.2f;
        return std::clamp(voxelSize, 0.01f, 3.0f);
    }

    void VoxelDownsample::Downsample(std::vector<GPUPointData>& output, const std::vector<CubeInstance>& cubes, float voxelSize) {
        PrepareInput(cubes);  

        output.clear();
        if (inputPoints.empty()) return;

        if (voxelSize <= 0.0f) {
            voxelSize = CalculateVoxelSize(inputPoints);
        }

        const size_t inputCount = inputPoints.size();
        const size_t maxOutputCount = inputCount;
        const size_t hashTableSize = inputCount * 4;

        // INPUT BUFFER
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputPointSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, inputCount * sizeof(GPUPointData), inputPoints.data(), GL_STATIC_READ);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputPointSSBO);

        // OUTPUT BUFFER
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputPointSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, maxOutputCount * sizeof(GPUPointData), nullptr, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputPointSSBO);

        // COUNTER BUFFER
        GLuint zero = 0;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputCounterSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), &zero, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, outputCounterSSBO);

        // HASH TABLE
        std::vector<GLuint> emptyHash(hashTableSize, 0u);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelHashSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, hashTableSize * sizeof(GLuint), emptyHash.data(), GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, voxelHashSSBO);

        // UNIFORMS
        glUseProgram(computeShader);
        glUniform1f(glGetUniformLocation(computeShader, "voxelSize"), voxelSize);
        glUniform1ui(glGetUniformLocation(computeShader, "maxOutputPoints"), static_cast<GLuint>(maxOutputCount));
        glUniform1ui(glGetUniformLocation(computeShader, "hashTableSize"), static_cast<GLuint>(hashTableSize));

        // DISPATCH
        GLuint workGroupsX = (static_cast<GLuint>(inputCount) + 63) / 64;
        glDispatchCompute(workGroupsX, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // READ COUNT
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputCounterSSBO);
        GLuint* countPtr = static_cast<GLuint*>(
            glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT)
        );
        GLuint finalCount = std::min(*countPtr, static_cast<GLuint>(maxOutputCount));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

        if (finalCount == 0) return;

        // READ OUTPUT
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputPointSSBO);
        GPUPointData* data = static_cast<GPUPointData*>(
            glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, finalCount * sizeof(GPUPointData), GL_MAP_READ_BIT)
        );

        output.assign(data, data + finalCount);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

}