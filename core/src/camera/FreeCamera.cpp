#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#include <Camera.hpp>
#include <FreeCamera.hpp>

FreeCamera::FreeCamera(int windowWidth, int windowHeight) : Camera(windowWidth, windowHeight) {}

void FreeCamera::Update(float deltaTime) {
    glm::vec3 cameraRight = glm::normalize(glm::cross(lookDirection, cameraUpDirection));
    glm::vec3 cameraUp = glm::cross(cameraRight, lookDirection);
    view = glm::lookAt(position, position + lookDirection, cameraUp);
}

void FreeCamera::ProcessKeyboard(float deltaTime) {
    const bool* keystate = SDL_GetKeyboardState(nullptr);
    if (!keystate[SDL_SCANCODE_W] &&
        !keystate[SDL_SCANCODE_S] && 
        !keystate[SDL_SCANCODE_A] && 
        !keystate[SDL_SCANCODE_D] && 
        !keystate[SDL_SCANCODE_SPACE]) {
        return;
    }

    glm::vec3 strafeDirection = glm::cross(lookDirection, cameraUpDirection);
    glm::vec3 moveDirection(0.0f);

    // MOVE UP WITH SPACEBAR
    if (keystate[SDL_SCANCODE_SPACE]) moveDirection += cameraUpDirection;

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
