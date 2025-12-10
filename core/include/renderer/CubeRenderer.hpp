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

    void Init();

    void UpdateBufferSize(int maxInstanceCount);

    void Render(bool enableCulling, glm::mat4 view, glm::mat4 projection);

    void AddCubes(const std::vector<Data::Point>& points);

    void UpdateCube(size_t index, const Data::Point& point);

    void Shutdown();
    
    void Clear();

    // ACCESSOR METHODS
    const std::vector<Data::Cube>& GetCubes();

    float& GetGlobalScale();
    glm::vec3& GetGlobalColor();
}