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
        
        glUseProgram(computeProgram);

        uVoxelSize = glGetUniformLocation(computeProgram, "uVoxelSize");
        uVoxelOrigin = glGetUniformLocation(computeProgram, "uVoxelOrigin");
        uVoxelBounds = glGetUniformLocation(computeProgram, "uVoxelBounds");
        uVoxelFlagSize = glGetUniformLocation(computeProgram, "uVoxelFlagSize");
    }

    // DECONSTRUCTOR
    VoxelDownsample::~VoxelDownsample() {
        if (computeProgram) glDeleteProgram(computeProgram);
        if (inputPointSSBO) glDeleteBuffers(1, &inputPointSSBO);
        if (outputFlagSSBO) glDeleteBuffers(1, &outputFlagSSBO);

        computeProgram = inputPointSSBO = outputFlagSSBO = 0;
    }

    void VoxelDownsample::CalculateVoxelSize() {
        if (pointData.empty()) return;

        glm::vec3 minPoint = pointData[0];
        glm::vec3 maxPoint = pointData[0];
        for (const glm::vec3& point : pointData) {
            minPoint = glm::min(minPoint, point);
            maxPoint = glm::max(maxPoint, point);
        }
        voxelOrigin = minPoint;

        // CALCULATE THE VOXEL SIZE
        float lowerBoundVoxelSize = 0.4f;
        float lowerBoundPointCount = 50'000.0f;
        float upperBoundPointCount = 8'000'000.0f;
        float upperBoundVoxelSize = 4.0f;
        
        float exponent = 
            std::log(upperBoundVoxelSize / lowerBoundVoxelSize) / 
            std::log(upperBoundPointCount / lowerBoundPointCount);
        float pointRatio = pointData.size() / lowerBoundPointCount;
        float size = lowerBoundVoxelSize * std::pow(pointRatio, exponent);   
        voxelSize = std::clamp(size, 0.25f, 6.0f);

        // CALCULATE VOXEL BOUNDS, AND VOXEL COUNT
        glm::vec3 extent = maxPoint - minPoint;
        int numVoxelsX = static_cast<int>(std::ceil(extent.x / voxelSize));
        int numVoxelsY = static_cast<int>(std::ceil(extent.y / voxelSize));
        int numVoxelsZ = static_cast<int>(std::ceil(extent.z / voxelSize));
        voxelBounds = glm::vec3(numVoxelsX, numVoxelsY, numVoxelsZ);
        voxelFlagSize = numVoxelsX * numVoxelsY * numVoxelsZ;
    }

    void VoxelDownsample::UpdateBufferSize() {
        // INPUT BUFFER (POINT POSITIONS)
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputPointSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, pointData.size() * sizeof(glm::vec3), pointData.data(), GL_DYNAMIC_DRAW);

        // OUTPUT BUFFER (KEEP/REMOVE FLAGS)
        std::vector<GLuint> outputFlags(voxelFlagSize, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputFlagSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, voxelFlagSize * sizeof(GLuint), outputFlags.data(), GL_DYNAMIC_DRAW);

        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
    }

    std::vector<CubeInstance> VoxelDownsample::ProcessPoints(std::vector<CubeInstance>& cubes) {
        if (cubes.empty()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
                "NO POINTS TO PROCESS IN VOXEL DOWNSAMPLING");
            return {};
        }
        if (cubes.size() > UINT32_MAX) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, 
                "DATASET TOO LARGE: %zu POINTS EXCEED UINT32 LIMIT", cubes.size());
            return {};
        }

        // PREPARE INPUT DATA
        pointData.clear();
        pointData.reserve(cubes.size());
        for (const CubeInstance& cube : cubes) {
            pointData.push_back(cube.position);
        }

        CalculateVoxelSize();
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, 
            "VOXEL SIZE: %.3f, VOXEL COUNT: %zu, POINT COUNT: %zu", voxelSize, voxelFlagSize, pointData.size());

        UpdateBufferSize();

        glUseProgram(computeProgram);

        // SET UNIFORMS
        glUniform1f(uVoxelSize, voxelSize);
        glUniform3fv(uVoxelOrigin, 1, glm::value_ptr(voxelOrigin));
        glUniform3fv(uVoxelBounds, 1, glm::value_ptr(voxelBounds));
        glUniform1ui(uVoxelFlagSize, static_cast<GLuint>(voxelFlagSize));

        // BIND BUFFERS
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputPointSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputFlagSSBO);

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
            if (outputFlags[index] > 0) {
                filteredCubes.push_back(cubes[index]);
            }
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        return filteredCubes;
    }

}