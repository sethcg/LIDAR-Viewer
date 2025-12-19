#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#include <Camera.hpp>

class OrbitalCamera : public Camera {
    public:
        OrbitalCamera(int windowWidth, int windowHeight);

        void Update(float deltaTime) override;

        float& GetRotationSpeed() { return rotationSpeed; }
        float& GetZoom() { return zoom; }
        float& GetTargetZoom() { return targetZoom; }
        float& GetMinZoom() { return minZoom; }
        float& GetMaxZoom() { return maxZoom; }

    private:
        float angle = 0.0f;
        float rotationSpeed = 15.0f;

        float zoom = 1.0f;
        float targetZoom = 1.0f;
        float zoomSpeed = 5.0f;
        float minZoom = 0.2f;
        float maxZoom = 5.0f;
};
