#include <algorithm>
#include <chrono>
#include <vector>
#include <string>
#include <cmath>

#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <CubeInstance.hpp>
#include <RendererHelper.hpp>
#include <VoxelDownsample.hpp>

namespace Filters {

    // CONSTRUCTOR
    VoxelDownsample::VoxelDownsample() {
        computeProgram = Renderer::CreateComputeShaderProgram("../assets/shaders/compute/voxel_downsample.comp");

        glGenBuffers(1, &inputPointSSBO);
        glGenBuffers(1, &outputFlagSSBO);
        glGenBuffers(1, &hashTableBuffer);
        
        glUseProgram(computeProgram);

        uVoxelSize = glGetUniformLocation(computeProgram, "uVoxelSize");
        uVoxelOrigin = glGetUniformLocation(computeProgram, "uVoxelOrigin");
        uHashTableSize = glGetUniformLocation(computeProgram, "uHashTableSize");
    }

    // DECONSTRUCTOR
    VoxelDownsample::~VoxelDownsample() {
        if (computeProgram) glDeleteProgram(computeProgram);
        if (inputPointSSBO) glDeleteBuffers(1, &inputPointSSBO);
        if (outputFlagSSBO) glDeleteBuffers(1, &outputFlagSSBO);
        if (hashTableBuffer) glDeleteBuffers(1, &hashTableBuffer);
        if (outputCounterBuffer) glDeleteBuffers(1, &outputCounterBuffer);

        computeProgram = 0;
        inputPointSSBO = 0;
        outputFlagSSBO = 0;
        hashTableBuffer = 0;
        outputCounterBuffer = 0;
    }

    void VoxelDownsample::CalculateVoxelSize() {
        if (pointData.empty()) return;

        glm::vec3 minPoint = pointData[0];
        for (const glm::vec3& point : pointData) {
            minPoint = glm::min(minPoint, point);
        }

        float baseVoxelSize = 0.35f;
        float basePointCount = 50'000.0f;
        float targetPointCount = 5'000'000.0f;
        float targetVoxelSize = 3.5f;
        
        float exponent = 
            std::log(targetVoxelSize / baseVoxelSize) / 
            std::log(targetPointCount / basePointCount);
        float pointRatio = pointData.size() / basePointCount;
        float calculatedSize = baseVoxelSize * std::pow(pointRatio, exponent);
        voxelSize = std::clamp(calculatedSize, 0.35f, 5.0f);
        
        voxelOrigin = minPoint;
        hashTableSize = pointData.size() * 8;
    }

    void VoxelDownsample::UpdateBufferSize() {
        // INPUT BUFFER (POINT POSITIONS)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputPointSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, pointData.size() * sizeof(glm::vec3), pointData.data(), GL_DYNAMIC_DRAW);

        // OUTPUT BUFFER (KEEP/REMOVE FLAGS)
        std::vector<GLuint> outputFlags(pointData.size(), 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputFlagSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, pointData.size() * sizeof(GLuint), outputFlags.data(), GL_DYNAMIC_DRAW);

        // VOXEL HASH TABLE BUFFER
        std::vector<GLuint> emptyHashTable(hashTableSize, 0); 
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, hashTableBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, hashTableSize * sizeof(GLuint), emptyHashTable.data(), GL_DYNAMIC_DRAW);

        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
    }

    void VoxelDownsample::ProcessPoints(std::vector<CubeInstance>& cubes) {
        if (cubes.empty()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
                "NO POINTS TO PROCESS IN VOXEL DOWNSAMPLING");
            return;
        }
        if (cubes.size() > UINT32_MAX) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
                "DATASET TOO LARGE: %zu POINTS EXCEED UINT32 LIMIT", cubes.size());
            return;
        }

        // PREPARE INPUT DATA
        pointData.clear();
        pointData.reserve(cubes.size());
        for (const CubeInstance& cube : cubes) {
            pointData.push_back(cube.position);
        }

        CalculateVoxelSize();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
            "VOXEL SIZE: %.3f, HASH TABLE SIZE: %u, POINT COUNT: %zu", 
            voxelSize, static_cast<GLuint>(hashTableSize), pointData.size());

        UpdateBufferSize();

        glUseProgram(computeProgram);

        // SET UNIFORMS
        glUniform1f(uVoxelSize, voxelSize);
        glUniform3f(uVoxelOrigin, voxelOrigin.x, voxelOrigin.y, voxelOrigin.z);
        glUniform1ui(uHashTableSize, static_cast<GLuint>(hashTableSize));

        // BIND BUFFERS
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputPointSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputFlagSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, hashTableBuffer);

        // DISPATCH COMPUTE SHADER
        GLuint workGroupsX = (static_cast<GLuint>(pointData.size()) + 63) / 64;
        glDispatchCompute(workGroupsX, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // READ OUTPUT FLAGS (1 KEEP, 0 REMOVE)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputFlagSSBO);
        GLuint* outputFlags = static_cast<GLuint*>(
            glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, pointData.size() * sizeof(GLuint), GL_MAP_READ_BIT)
        );

        // REMOVE POINTS BASED ON OUTPUT FLAGS
        std::vector<CubeInstance> filteredCubes;
        filteredCubes.reserve(cubes.size());
        for (GLuint index = 0; index < cubes.size(); ++index) {
            // KEEP POINT IF FLAGGED
            if (outputFlags[index] == 1) {
                filteredCubes.push_back(cubes[index]);
            }
        }
    
        // UPDATE CUBES TO REFLECT DOWNSAMPLED POINTS
        cubes = std::move(filteredCubes);

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

}