#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
    public:
        Camera(int windowWidth, int windowHeight) { Resize(windowWidth, windowHeight); }
        virtual ~Camera() = default;

        virtual void Update(float deltaTime) = 0;
        virtual void ProcessKeyboard(float deltaTime) {}
        virtual void ProcessMouseMotion(float xrel, float yrel) {}

        virtual void UpdateBounds(glm::vec3 center, float radius) {
            sceneCenter = center;
            sceneRadius = radius;
        }

        virtual void Resize(int windowWidth, int windowHeight) {
            projection = glm::perspective(
                glm::radians(45.0f),
                float(windowWidth) / float(windowHeight),
                0.1f,           // NEAR
                100000.0f       // FAR
            );
        }

        glm::mat4 GetViewProjection() const { return projection * view; }

    protected:
        glm::mat4 view;
        glm::mat4 projection;

        glm::vec3 sceneCenter = glm::vec3(0.0f);
        float sceneRadius = 5.0f;
};
