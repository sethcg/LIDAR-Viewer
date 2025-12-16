#pragma once

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Cube.hpp>
#include <RendererHelper.hpp>

// FORWARD DECLARATION
class Camera;

class CubeRenderer {
    public:
        CubeRenderer();
        ~CubeRenderer();

        inline void Init();
        inline void Shutdown();

        void Render(Camera& camera, float globalScale, glm::vec3 globalColor);

        void UpdateBufferSize(uint64_t pointCount);

        void UpdateBuffers();

        void AddCube(
            uint64_t index, 
            glm::vec3 position, 
            glm::vec3 color,
            uint16_t intensity
        );

        void NormalizeColors();

        void UpdateInstancePosition(uint64_t index, glm::vec3 position);

        void UpdateInstanceColor(uint64_t index, glm::vec3 color);
        
        void Clear();

        // ACCESSORS
        std::vector<Data::Cube>& GetCubes();

    private:
        std::vector<Data::Cube> cubes;

        size_t instanceCount = 0;
        std::vector<glm::mat4> instanceModels;
        std::vector<glm::vec3> instanceColors;

        // GPU OBJECTS
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLuint instanceVBO = 0;
        GLuint instanceColorVBO = 0;
        GLuint shaderProgram = 0;

        // SHADER UNIFORMS
        GLint uViewProjectionLocation = -1;
        GLint uGlobalColorLocation = -1;
        GLint uGlobalScaleLocation = -1;

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
        CubeRenderer& operator=(const CubeRenderer&) = delete;
};
