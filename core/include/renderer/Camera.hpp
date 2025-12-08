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

        const glm::mat4& GetView() const { return view; }
        const glm::mat4& GetProjection() const { return projection; }

        float& GetRotationSpeed() { return rotationSpeed; }
        void SetRotationSpeed(float value) { rotationSpeed = value; }
        
        float& GetZoom() { return zoom; }
        float& GetTargetZoom() { return targetZoom; }
        void SetTargetZoom(float value) { targetZoom = value; }
        float& GetMinZoom() { return minZoom; }
        float& GetMaxZoom() { return maxZoom; }

    private:
        glm::mat4 view;
        glm::mat4 projection;

        glm::vec3 sceneCenter = glm::vec3(0.0f);
        float sceneDistance = 5.0f;
        float sceneRadius = 5.0f;

        // CAMERA ROTATION
        float angle = 0.0f;
        float rotationSpeed = 30.0f;

        // CAMERA ZOOM
        float zoom = 1.0f;
        float targetZoom = 1.0f;
        float zoomSpeed = 5.0f;
        float minZoom = 0.2f;
        float maxZoom = 5.0f;
};
