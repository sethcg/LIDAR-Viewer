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

    void UpdateBufferData();

    void Render(glm::mat4 view, glm::mat4 projection);

    void AddCubes(const std::vector<Data::Point>& points);

    void UpdateCube(size_t index, const Data::Point& point);

    void Shutdown();
    
    void Clear();

    // ACCESSOR METHODS
    std::vector<Data::Cube>& GetCubes();
    size_t& GetInstanceCount();
    std::vector<glm::mat4>& GetInstanceModels();
    std::vector<glm::vec3>& GetInstanceColors();

    float& GetGlobalScale();
    glm::vec3& GetGlobalColor();
}