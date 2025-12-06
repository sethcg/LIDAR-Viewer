#include <vector>
#include <cmath>
#include <cfloat>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include <glad/glad.h>

#include <App.hpp>
#include <Camera.hpp>
#include <CubeRenderer.hpp>

Camera::Camera(int windowWidth, int windowHeight) {
    RecalculateBounds();
    Resize(windowWidth, windowHeight);
}

void Camera::RecalculateBounds() {
    const auto& cubes = CubeRenderer::GetCubes();
    if (cubes.empty()) {
        sceneCenter = glm::vec3(0, 0, 0);
        sceneDistance = 5.0f;
        sceneRadius = 5.0f;
        return;
    }

    glm::vec3 minDist(FLT_MAX);
    glm::vec3 maxDist(-FLT_MAX);

    for (const auto& cube : cubes) {
        float scale = cube.scale * CubeRenderer::GetGlobalScale();
        glm::vec3 offset(scale);

        // COMPONENT-WISE MIN/MAX (USING "glm::min/max" WAS THROWING ERRORS)
        minDist.x = std::fmin(minDist.x, cube.position.x - offset.x);
        minDist.y = std::fmin(minDist.y, cube.position.y - offset.y);
        minDist.z = std::fmin(minDist.z, cube.position.z - offset.z);

        maxDist.x = std::fmax(maxDist.x, cube.position.x + offset.x);
        maxDist.y = std::fmax(maxDist.y, cube.position.y + offset.y);
        maxDist.z = std::fmax(maxDist.z, cube.position.z + offset.z);
    }

    sceneCenter = 0.5f * (minDist + maxDist);

    float extent = std::fmax(maxDist.x - minDist.x, maxDist.y - minDist.y) * 0.5f;
    float fov = 45.0f;
    sceneDistance = extent / tan(glm::radians(fov * 0.5f));
    sceneRadius = sceneDistance * 1.3f;
}

void Camera::Resize(int windowWidth, int windowHeight) {
    glViewport(0, 0, windowWidth, windowHeight);
    projection = glm::perspective(glm::radians(45.0f), float(windowWidth) / windowHeight, 0.1f, 1000.0f);
}

void Camera::Update(float deltaTime) {
    if (deltaTime > 0.1f) deltaTime = 0.1f;

    // CAMERA ORBITS AROUND THE CENTER
    angle += rotationSpeed * deltaTime;

    // SMOOTH ZOOM
    zoom = glm::mix(zoom, targetZoom, deltaTime * zoomSpeed);

    float radians = glm::radians(angle);
    float distance = sceneRadius / zoom;

    glm::vec3 camPos;
    camPos.x = sceneCenter.x + distance * cos(radians);
    camPos.y = sceneCenter.y + distance * sin(radians);
    camPos.z = sceneCenter.z + (sceneDistance * 0.5f) / zoom;

    view = glm::lookAt(camPos, sceneCenter, glm::vec3(0, 0, 1));
}
