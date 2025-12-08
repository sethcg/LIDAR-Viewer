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
        sceneCenter = glm::vec3(0.0f);
        sceneRadius = 5.0f;
        return;
    }

    glm::vec3 minDist( FLT_MAX);
    glm::vec3 maxDist(-FLT_MAX);

    for (const auto& cube : cubes) {
        float scale = cube.scale * CubeRenderer::GetGlobalScale();
        glm::vec3 offset(scale);
        glm::vec3 minP = cube.position - offset;
        glm::vec3 maxP = cube.position + offset;
        minDist = glm::min(minDist, minP);
        maxDist = glm::max(maxDist, maxP);
    }

    // CENTER OF BOUNDING BOX
    sceneCenter = 0.5f * (minDist + maxDist);

    glm::vec3 size = maxDist - minDist;
    sceneRadius = glm::length(size) * 0.5f;
}

void Camera::Resize(int windowWidth, int windowHeight) {
    glViewport(0, 0, windowWidth, windowHeight);

    projection = glm::perspective(
        glm::radians(45.0f),
        float(windowWidth) / float(windowHeight),
        0.1f,
        100000.0f   // LARGE "RENDER DISTANCE"
    );
}

void Camera::Update(float deltaTime) {
    if (deltaTime > 0.1f) {
        deltaTime = 0.1f;
    }

    // SMOOTH ZOOM TRANSITION
    zoom = glm::mix(zoom, targetZoom, deltaTime * zoomSpeed);

    // ORBIT ROTATION
    angle += rotationSpeed * deltaTime;

    float theta = glm::radians(angle);     // HORIZONTAL ORBIT
    float phi = glm::radians(35.0f);     // CONSTANT ELEVATION

    float radius = sceneRadius / zoom;

    glm::vec3 camPos;
    camPos.x = sceneCenter.x + radius * cos(phi) * cos(theta);
    camPos.y = sceneCenter.y + radius * cos(phi) * sin(theta);
    camPos.z = sceneCenter.z + radius * sin(phi);

    view = glm::lookAt(camPos, sceneCenter, glm::vec3(0, 0, 1));
}
