#pragma once

#include <vector>
#include <cmath>
#include <cfloat>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>
#include <glad/glad.h>

#include <App.hpp>
#include <CubeRenderer.hpp>

class Camera {
    public:
        Camera(int windowWidth, int windowHeight);

        void RecalculateBounds();
        void Resize(int windowWidth, int windowHeight);
        void Update(float deltaTime);

        void ProcessKeyboard(float deltaTime);
        void ProcessMouseMotion(float xrel, float yrel);

        const glm::mat4& GetView() const { return view; }
        const glm::mat4& GetProjection() const { return projection; }

        float& GetRotationSpeed() { return rotationSpeed; }
        float& GetZoom() { return zoom; }
        float& GetTargetZoom() { return targetZoom; }
        float& GetMinZoom() { return minZoom; }
        float& GetMaxZoom() { return maxZoom; }

        bool& GetFreeCamera() { return freeCamera; }

    private:
        glm::mat4 view;
        glm::mat4 projection;

        glm::vec3 sceneCenter = glm::vec3(0.0f);
        float sceneDistance = 5.0f;
        float sceneRadius = 5.0f;

        // ORBIT CAMERA ROTATION
        float angle = 0.0f;
        float rotationSpeed = 30.0f;

        // ORBIT CAMERA ZOOM
        float zoom = 1.0f;
        float targetZoom = 1.0f;
        float zoomSpeed = 5.0f;
        float minZoom = 0.2f;
        float maxZoom = 5.0f;

        // FREE CAMERA
        bool freeCamera = false;
        glm::vec3 position = glm::vec3(0.0f, -10.0f, 5.0f);
        glm::vec3 lookDirection = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraUpDirection = glm::vec3(0.0f, 0.0f, 1.0f);

        float horizontalAngle = 90.0f;      // YAW
        float verticalAngle = 0.0f;         // PITCH
        float movementSpeed = 20.0f;
        float accelerationSpeed = 2.0f;
        float mouseSensitivity = 0.1f;
};
