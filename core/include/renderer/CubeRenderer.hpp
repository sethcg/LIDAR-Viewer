#pragma once

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

#include <ColorLUT.hpp>
#include <ColorRamp.hpp>
#include <CubeInstance.hpp>
#include <RendererHelper.hpp>
#include <VoxelDownsampleFilter.hpp>

// FORWARD DECLARATION
class Camera;

using namespace Renderer;

class CubeRenderer {
    public:
        // CONSTRUCTOR / DESTRUCTOR
        CubeRenderer() = default;
        ~CubeRenderer() { Shutdown(); }

        void Init(Data::ColorRampType rampType);
        void Shutdown();

        void Render(const glm::mat4& viewProjection, float globalScale);
        void UpdateBufferSize(uint64_t pointCount);
        void UpdateBuffers();

        void AddCube(glm::vec3 position, uint16_t intensity);
        void UpdateInstancePosition(uint64_t index, glm::vec3 position);
        void UpdateInstanceIntensity(uint64_t index, float intensity);

        void NormalizeIntensities();
        void UpdateColorRamp(Data::ColorRampType rampType);
        
        // GPU FILTERS
        void VoxelDownsample();

        void Clear();

    private:
        Utils::ColorLUT colorLUT;

        std::vector<CubeInstance> cubes;

        // INSTANCE BUFFERS
        std::vector<glm::mat4> instanceModels;
        std::vector<float> instanceIntensities;

        // GPU UNIFORMS
        GLint uViewProjectionLocation = -1;
        GLint uGlobalScaleLocation = -1;

        // GPU RESOURCES
        GLuint cubeShader = 0;
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLuint instanceVBO = 0;
        GLuint instanceIntensityVBO = 0;
        
        // FILTERS
        Filters::VoxelDownsampleFilter voxelDownsampleFilter;

        // CUBE VERTICES (CORNER POSITIONS)
        static constexpr float cubeVertices[24] = {
            -0.5f, -0.5f, -0.5f,    // 1
            0.5f, -0.5f, -0.5f,     // 2
            0.5f,  0.5f, -0.5f,     // 3
            -0.5f,  0.5f, -0.5f,    // 4
            -0.5f, -0.5f,  0.5f,    // 5
            0.5f, -0.5f,  0.5f,     // 6
            0.5f,  0.5f,  0.5f,     // 7
            -0.5f,  0.5f,  0.5f     // 8
        };

        // INDICES FOR TRIANGLES (6 FACES)
        static constexpr unsigned int cubeIndices[36] = {
            0, 1, 2, 2, 3, 0,   // BACK
            5, 4, 7, 7, 6, 5,   // FRONT
            4, 0, 3, 3, 7, 4,   // LEFT
            1, 5, 6, 6, 2, 1,   // RIGHT
            3, 2, 6, 6, 7, 3,   // TOP
            4, 5, 1, 1, 0, 4    // BOTTOM
        };
        
    private:
        // NON-COPYABLE (OWNS GPU RESOURCES)
        CubeRenderer(const CubeRenderer&) = delete;
        CubeRenderer& operator = (const CubeRenderer&) = delete;
};
