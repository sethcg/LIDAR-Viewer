#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#include <Camera.hpp>

class FreeCamera : public Camera {
    public:
        FreeCamera(int windowWidth, int windowHeight);

        void Update(float deltaTime) override;
        void ProcessKeyboard(float deltaTime) override;
        void ProcessMouseMotion(float xrel, float yrel) override;
        void UpdateBounds(glm::vec3 center, float radius) override;

        float& GetSpeedFactor() { return speedFactor; }
        float& GetAccelerationFactor() { return accelerationFactor; }
        float& GetMouseSensitivityFactor() { return mouseSensitivityFactor; }

    private:
        void PositionCamera(float radius);
        float ComputeMovementSpeed() const;

        glm::vec3 position = glm::vec3(0.0f, -10.0f, 5.0f);
        glm::vec3 lookDirection = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraUpDirection = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 currentVelocity = glm::vec3(0.0f);

        float horizontalAngle = 90.0f;
        float verticalAngle = 0.0f;

        float speedFactor = 0.5f;
        float movementSpeed = 0.0f;
        float minMovementSpeed = 0.0f;
        float maxMovementSpeed = 1.0f;

        float accelerationFactor = 0.75f;
        float mouseSensitivityFactor = 0.5f;
};
