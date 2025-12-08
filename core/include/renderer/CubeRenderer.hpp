#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Cube.hpp>
#include <CubeRenderer.hpp>
#include <RendererHelper.hpp>
#include <Point.hpp>

namespace CubeRenderer {

    // TODO: PERFORMANCE IMPROVEMENT TO USE "glm::vec3" AND CUBE INDICES
    // AND ALLOW FOR USING "glDrawElements" INSTEAD OF "glDrawArrays"
    const float vertices[] = {
        // BACK FACE
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,

        // FRONT FACE
        -0.5f,-0.5f, 0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,

        // LEFT FACE
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        // RIGHT FACE
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        // BOTTOM FACE
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,

        // TOP FACE
        -0.5f, 0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
    };

    void Init();

    void UpdateBufferSize(int maxInstanceCount);

    void Render(glm::mat4 view, glm::mat4 projection);

    void AddCubes(const std::vector<Data::Point>& points);

    void UpdateCube(size_t index, const Data::Point& point);

    void Shutdown();
    
    void Clear();

    // ACCESSOR METHODS
    const std::vector<Data::Cube>& GetCubes();

    float& GetGlobalScale();
    glm::vec3& GetGlobalColor();
}