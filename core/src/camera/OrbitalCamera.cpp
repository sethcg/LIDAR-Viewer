#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#include <Camera.hpp>
#include <OrbitalCamera.hpp>

OrbitalCamera::OrbitalCamera(int windowWidth, int windowHeight) : Camera(windowWidth, windowHeight) {}

void OrbitalCamera::Update(float deltaTime) {
    if (deltaTime > 0.1f) deltaTime = 0.1f;

    // SMOOTH ZOOM
    zoom = glm::mix(zoom, targetZoom, deltaTime * zoomSpeed);
    float radius = sceneRadius / zoom;

    // SMOOTH ORBIT ROTATION
    angle += rotationSpeed * deltaTime;

    glm::quat horizontalRotation = glm::angleAxis(glm::radians(angle), glm::vec3(0, 0, 1));
    float targetElevation = glm::radians(35.0f);
    glm::quat elevationRotation = glm::angleAxis(targetElevation, glm::vec3(1, 0, 0));

    glm::quat rotation = horizontalRotation * elevationRotation;
    glm::vec3 offset = glm::vec3(0, radius, 0);

    glm::vec3 cameraPosition = sceneCenter + rotation * offset;
    view = glm::lookAt(cameraPosition, sceneCenter, glm::vec3(0, 0, 1));
}
