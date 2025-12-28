#include <chrono>
#include <vector>
#include <string>

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ColorLUT.hpp>
#include <ColorRamp.hpp>
#include <CubeInstance.hpp>
#include <CubeRenderer.hpp>
#include <RendererHelper.hpp>
#include <VoxelDownsampleFilter.hpp>

using namespace Renderer;

void CubeRenderer::Init(Data::ColorRampType rampType) {
    colorLUT.Init(rampType);
    cubeShader = CreateShaderProgramFromFiles(
        "../assets/shaders/cube/cube.vert",
        "../assets/shaders/cube/cube.frag"
    );

    glUseProgram(cubeShader);
    uViewProjectionLocation = glGetUniformLocation(cubeShader, "uViewProjection");
    uGlobalScaleLocation = glGetUniformLocation(cubeShader, "uGlobalScale");
    glUseProgram(0);

    // SETUP VAO, VBO, EBO, INSTANCE VARIABLES
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &instanceVBO);
    glGenBuffers(1, &instanceIntensityVBO);

    glBindVertexArray(vao);

    // CUBE VERTEX POSITIONS
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,3 * sizeof(float), (void*) 0);

    // CUBE INDICES
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // SETUP INSTANCE MODEL MATRIX BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    for (int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(1 + i);
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),(void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(1 + i, 1);
    }

    // SETUP INSTANCE INTENSITY BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, instanceIntensityVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);
}

void CubeRenderer::Shutdown() {
    if (cubeShader) glDeleteProgram(cubeShader);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
    if (instanceVBO) glDeleteBuffers(1, &instanceVBO);
    if (instanceIntensityVBO) glDeleteBuffers(1, &instanceIntensityVBO);
    
    colorLUT.Shutdown();
    
    vao = vbo = ebo = instanceVBO = instanceIntensityVBO = cubeShader;
}

void CubeRenderer::Render(const glm::mat4& viewProjection, float globalScale) {
    if(cubes.empty()) return;

    glEnable(GL_DEPTH_TEST);

    glUseProgram(cubeShader);
    glBindVertexArray(vao);

    glUniformMatrix4fv(uViewProjectionLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform1f(uGlobalScaleLocation, globalScale);

    colorLUT.Bind(0);
    glUniform1i(glGetUniformLocation(cubeShader, "uColorLUT"), 0);

    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(cubes.size()));

    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_DEPTH_TEST);
}

void CubeRenderer::UpdateBufferSize(uint64_t pointCount) {
    cubes.clear();
    cubes.reserve(pointCount);

    instanceModels.resize(pointCount);
    instanceIntensities.resize(pointCount);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, instanceIntensityVBO);
    glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof(float), instanceIntensities.data(), GL_DYNAMIC_DRAW);
}

void CubeRenderer::UpdateBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceModels.size() * sizeof(glm::mat4), instanceModels.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, instanceIntensityVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceIntensities.size() * sizeof(float), instanceIntensities.data(), GL_DYNAMIC_DRAW);
}

void CubeRenderer::AddCube(glm::vec3 position, uint16_t intensity) {
    const uint64_t index = cubes.size();

    // ADD CUBE
    cubes.emplace_back(position, intensity);

    // UPDATE INSTANCE BUFFERS
    UpdateInstancePosition(index, position);
    UpdateInstanceIntensity(index, intensity);
}

void CubeRenderer::UpdateInstancePosition(uint64_t index, glm::vec3 position) {
    instanceModels[index] = glm::translate(glm::mat4(1.0f), position);
}

void CubeRenderer::UpdateInstanceIntensity(uint64_t index, float intensity) {
    instanceIntensities[index] = intensity;
}

void CubeRenderer::NormalizeIntensities() {
    if (cubes.empty()) return;

    const size_t BIN_COUNT = 65535;
    std::vector<size_t> histogram(BIN_COUNT, 0);

    // BUILD COLOR HISTOGRAM
    for (auto& cube : cubes) {
        histogram[cube.intensity]++;
    }

    // BUILD CUMULATIVE HISTOGRAM (CUMULATIVE DISTRIBUTION FUNCTION)
    std::vector<size_t> cumulativeHistogram(BIN_COUNT, 0);
    cumulativeHistogram[0] = histogram[0];
    for (size_t i = 1; i < BIN_COUNT; ++i) {
        cumulativeHistogram[i] = cumulativeHistogram[i - 1] + histogram[i];
    }

    const float size_inv = 1.0f / float(cubes.size());
    for (size_t i = 0; i < cubes.size(); ++i) {
        float normalized = float(cumulativeHistogram[cubes[i].intensity]) * size_inv;
        cubes[i].normalized_intensity = normalized;
        instanceIntensities[i] = normalized;
    }
}

void CubeRenderer::UpdateColorRamp(Data::ColorRampType rampType) {
    colorLUT.Update(rampType);
}

void CubeRenderer::VoxelDownsample() {
    if (cubes.empty()) return;
    auto start = std::chrono::steady_clock::now();

    uint64_t inputCount = cubes.size();

    // EXECUTE VOXEL DOWNSAMPLING FILTER
    std::vector<CubeInstance> filteredCubes = voxelDownsampleFilter.ProcessPoints(cubes);
    if (filteredCubes.empty()) return;

    UpdateBufferSize(filteredCubes.size());
    for (const CubeInstance& cube : filteredCubes) {
        AddCube(cube.position, cube.intensity);
    }
    UpdateBuffers();

    // UPDATE INSTANCE BUFFERS
    instanceModels.resize(cubes.size());
    instanceIntensities.resize(cubes.size());
    for (size_t i = 0; i < cubes.size(); ++i) {
        instanceModels[i] = glm::translate(glm::mat4(1.0f), cubes[i].position);
        instanceIntensities[i] = cubes[i].intensity;
    }

    auto end = std::chrono::steady_clock::now();
    double seconds = std::chrono::duration<double>(end - start).count();
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION,
        "GPU VOXEL DOWNSAMPLING: %zu -> %zu POINTS (%.1f%% REDUCTION) IN %.4f SECONDS",
        inputCount, cubes.size(),
        (1.0f - static_cast<float>(cubes.size()) / static_cast<float>(inputCount)) * 100.0f,
        seconds);
}

void CubeRenderer::Clear() {
    // CLEAR CPU INSTANCE INFORMATION
    cubes.clear();
    instanceModels.clear();
    instanceIntensities.clear();

    // FLUSH GPU BUFFERS
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, instanceIntensityVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
}