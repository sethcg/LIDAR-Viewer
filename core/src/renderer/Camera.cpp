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
        0.1f,           // NEAR
        100000.0f       // FAR
    );
}

void Camera::Update(float deltaTime) {
    if(freeCamera) {
        glm::vec3 cameraRight = glm::normalize(glm::cross(lookDirection, cameraUpDirection));
        glm::vec3 cameraUp = glm::cross(cameraRight, lookDirection);
        view = glm::lookAt(position, position + lookDirection, cameraUp);
    } else {
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
}

void Camera::ProcessKeyboard(float deltaTime) {
    if(!freeCamera) return;

    const bool* keystate = SDL_GetKeyboardState(nullptr);
    if (!keystate[SDL_SCANCODE_W] &&
        !keystate[SDL_SCANCODE_S] && 
        !keystate[SDL_SCANCODE_A] && 
        !keystate[SDL_SCANCODE_D]) {
        return;
    }

    glm::vec3 strafeDirection = glm::cross(lookDirection, cameraUpDirection);
    glm::vec3 moveDirection(0.0f);

    // MOVE FORWARD/BACKWARD (DEPENDING ON FACING DIRECTION)
    if (keystate[SDL_SCANCODE_W]) moveDirection += lookDirection;
    if (keystate[SDL_SCANCODE_S]) moveDirection -= lookDirection;

    // STRAFE LEFT/RIGHT
    if (keystate[SDL_SCANCODE_D]) moveDirection += strafeDirection;
    if (keystate[SDL_SCANCODE_A]) moveDirection -= strafeDirection;

    // APPLY MOVEMENT TO POSITION
    if (glm::length(moveDirection) > 0.0f) {
        float speedMultiplier = keystate[SDL_SCANCODE_LSHIFT] ? accelerationSpeed : 1.0f;
        position += moveDirection * movementSpeed * deltaTime * speedMultiplier;
    }
}

void Camera::ProcessMouseMotion(float xrel, float yrel) {
    if (!freeCamera) return;
    if (xrel == 0.0f && yrel == 0.0f) return;

    // ADJUST HORIZONTAL (YAW) ANGLE
    horizontalAngle += xrel * mouseSensitivity;

    // ADJUST/CLAMP VERTICAL (PITCH) ANGLE TO PREVENT CAMERA FLIPPING UPSIDE DOWN
    verticalAngle = glm::clamp(verticalAngle - yrel * mouseSensitivity, -89.0f, 89.0f);

    float pitch = glm::radians(verticalAngle);
    float yaw   = glm::radians(horizontalAngle);
    lookDirection.x = cos(pitch) * sin(yaw);
    lookDirection.y = cos(pitch) * cos(yaw);
    lookDirection.z = sin(pitch);
}