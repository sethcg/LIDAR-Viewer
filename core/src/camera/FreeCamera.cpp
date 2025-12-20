#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#include <Camera.hpp>
#include <FreeCamera.hpp>

FreeCamera::FreeCamera(int windowWidth, int windowHeight) : Camera(windowWidth, windowHeight) {}

void FreeCamera::UpdateBounds(glm::vec3 center, float radius) {
    Camera::UpdateBounds(center, radius);
    PositionCamera(radius);
    // MAX MOVEMENT: ~40% OF RADIUS/SEC, CAPPED AT 500
    // MIN MOVEMENT: ~15% OF RADIUS/SEC, CAPPED AT 15% OF MAX SPEED
    maxMovementSpeed = glm::min(sceneRadius * 0.40f, 500.0f);
    minMovementSpeed = glm::min(sceneRadius * 0.15f, maxMovementSpeed * 0.15f);
    movementSpeed = ComputeMovementSpeed();
}

void FreeCamera::PositionCamera(float radius) {
    position = glm::vec3(radius, -(radius * 0.5f), (radius * 0.5f));
    lookDirection = glm::normalize(sceneCenter - position);

    if (glm::length(lookDirection) < 0.0001f) return;

    glm::vec3 direction = glm::normalize(lookDirection);
    verticalAngle = glm::degrees(asin(direction.z));
    horizontalAngle = glm::degrees(atan2(direction.x, direction.y));
}

float FreeCamera::ComputeMovementSpeed() const {
    return glm::mix(minMovementSpeed, maxMovementSpeed, speedFactor);
}

void FreeCamera::Update(float deltaTime) {
    glm::vec3 cameraRight = glm::normalize(glm::cross(lookDirection, cameraUpDirection));
    glm::vec3 cameraUp = glm::cross(cameraRight, lookDirection);
    view = glm::lookAt(position, position + lookDirection, cameraUp);
}

void FreeCamera::ProcessKeyboard(float deltaTime) {
    const bool* keystate = SDL_GetKeyboardState(nullptr);
    
    glm::vec3 strafeDirection = glm::cross(lookDirection, cameraUpDirection);
    glm::vec3 moveDirection(0.0f);

    // DETERMINE DESIRED MOVEMENT DIRECTION
    if (keystate[SDL_SCANCODE_SPACE]) moveDirection += cameraUpDirection;
    if (keystate[SDL_SCANCODE_W]) moveDirection += lookDirection;
    if (keystate[SDL_SCANCODE_S]) moveDirection -= lookDirection;
    if (keystate[SDL_SCANCODE_D]) moveDirection += strafeDirection;
    if (keystate[SDL_SCANCODE_A]) moveDirection -= strafeDirection;

    if (glm::length(moveDirection) > 0.0f) {
        float movementSpeed = ComputeMovementSpeed();
        float accelerationSpeed = glm::clamp(accelerationFactor * 4.0f, 1.0f, 4.0f);
        float accelerationRate = keystate[SDL_SCANCODE_LSHIFT] ? accelerationSpeed : 1.0f;
        position += moveDirection * movementSpeed * accelerationRate * deltaTime;
    }
}

void FreeCamera::ProcessMouseMotion(float xrel, float yrel) {
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
