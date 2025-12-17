#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Camera.hpp>
#include <ColorRamp.hpp>
#include <Cube.hpp>
#include <CubeRenderer.hpp>
#include <RendererHelper.hpp>

void CubeRenderer::Init(const std::vector<glm::vec3>& ramp) {
    colorRamp = ramp;
    cubeShader = Renderer::CreateShaderProgramFromFiles(
        "../assets/shaders/cube/cube.vert",
        "../assets/shaders/cube/cube.frag"
    );

    glUseProgram(cubeShader);
    uViewProjectionLocation = glGetUniformLocation(cubeShader, "uViewProjection");
    uGlobalColorLocation = glGetUniformLocation(cubeShader, "uGlobalColor");
    uGlobalScaleLocation = glGetUniformLocation(cubeShader, "uGlobalScale");
    glUseProgram(0);

    // SETUP VAO, VBO, EBO, INSTANCE VARIABLES
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &instanceVBO);
    glGenBuffers(1, &instanceColorVBO);

    glBindVertexArray(vao);

    // CUBE VERTEX POSITIONS
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,3 * sizeof(float), (void*) 0);

    // CUBE INDICES
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // SETUP INSTANCE MODEL MATRIX BUFFERS
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    for (int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(1 + i);
        glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),(void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(1 + i, 1);
    }

    // SETUP INSTANCE COLOR BUFFERS
    glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);
}

void CubeRenderer::Shutdown() {
    if (cubeShader) glDeleteProgram(cubeShader);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
    if (instanceVBO) glDeleteBuffers(1, &instanceVBO);
    if (instanceColorVBO) glDeleteBuffers(1, &instanceColorVBO);
    vao = vbo = ebo = instanceVBO = instanceColorVBO = cubeShader = 0;
}

void CubeRenderer::Render(Camera& camera, float globalScale, glm::vec3 globalColor) {
    if (cubes.empty()) return;

    const glm::mat4& view = camera.GetView();
    const glm::mat4& projection = camera.GetProjection();

    glEnable(GL_DEPTH_TEST);

    glUseProgram(cubeShader);
    glBindVertexArray(vao);

    glm::mat4 viewProjection = projection * view;

    glUniformMatrix4fv(uViewProjectionLocation, 1, GL_FALSE, glm::value_ptr(viewProjection));
    glUniform3fv(uGlobalColorLocation, 1, glm::value_ptr(globalColor));
    glUniform1f(uGlobalScaleLocation, globalScale);

    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(cubes.size()));

    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_DEPTH_TEST);
}

void CubeRenderer::UpdateBufferSize(uint64_t pointCount) {
    cubes.clear();
    cubes.reserve(pointCount);

    instanceModels.resize(pointCount);
    instanceColors.resize(pointCount);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
    glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
}

void CubeRenderer::AddCube(glm::vec3 position, glm::vec3 color, uint16_t intensity) {
    const uint64_t index = cubes.size();

    // ADD CUBE
    cubes.emplace_back(position, color, intensity);

    // UPDATE INSTANCE BUFFERS
    UpdateInstancePosition(index, position);
    UpdateInstanceColor(index, color);
}

void CubeRenderer::UpdateInstancePosition(uint64_t index, glm::vec3 position) {
    instanceModels[index] = glm::translate(glm::mat4(1.0f), position);
}

void CubeRenderer::UpdateInstanceColor(uint64_t index, glm::vec3 color) {
    instanceColors[index] = color;
}

void CubeRenderer::NormalizeColors() {
    const size_t BIN_COUNT = 65535;
    std::vector<size_t> histogram(BIN_COUNT, 0);
    
    uint16_t min_intensity = UINT16_MAX;
    uint16_t max_intensity = 0;

    for (uint64_t index = 0; index < cubes.size(); ++index) {
        // BUILD COLOR HISTOGRAM
        uint16_t intensity = cubes[index].intensity;
        min_intensity = std::min(min_intensity, cubes[index].intensity);
        max_intensity = std::max(max_intensity, cubes[index].intensity);
        histogram[cubes[index].intensity]++;
    }

    // BUILD CUMULATIVE HISTOGRAM (CUMULATIVE DISTRIBUTION FUNCTION)
    std::vector<size_t> cumulativeHistogram(BIN_COUNT, 0);
    cumulativeHistogram[0] = histogram[0];
    for (size_t i = 1; i < BIN_COUNT; ++i) {
        cumulativeHistogram[i] = cumulativeHistogram[i - 1] + histogram[i];
    }

    // APPLY COLOR GRADIENT, WITH WHITE BEING THE FALLBACK COLOR
    auto ComputeColor = [&](uint64_t index) -> glm::vec3 {
        if (colorRamp.empty()) {
            return glm::vec3(1.0f);
        }
        float normalizedValue = float(cumulativeHistogram[cubes[index].intensity]) / float(cubes.size());
        cubes[index].normalized_intensity = normalizedValue;
        return Data::ColorMap(normalizedValue, colorRamp);
    };
    for (uint64_t index = 0; index < cubes.size(); ++index) {
        UpdateInstanceColor(index, ComputeColor(index));
    }
}

void CubeRenderer::UpdateBuffers() {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceModels.size() * sizeof(glm::mat4), instanceModels.data());

    glBindBuffer(GL_ARRAY_BUFFER, instanceColorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceColors.size() * sizeof(glm::vec3), instanceColors.data());
}

void CubeRenderer::UpdateColorRamp(const std::vector<glm::vec3>& ramp) {
    colorRamp = ramp;
    auto ComputeColor = [&](uint64_t index) -> glm::vec3 {
        if (colorRamp.empty()) {
            return glm::vec3(1.0f);
        }
        return Data::ColorMap(cubes[index].normalized_intensity, colorRamp);
    };
    for (uint64_t index = 0; index < cubes.size(); ++index) {
        UpdateInstanceColor(index, ComputeColor(index));
    }
}

void CubeRenderer::Clear() {
    cubes.clear();
    instanceModels.clear();
    instanceColors.clear();
}

// ACCESSORS
std::vector<Data::Cube>& CubeRenderer::GetCubes() { return cubes; }
