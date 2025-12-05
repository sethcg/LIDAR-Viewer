#include <glm/gtc/matrix_transform.hpp>

#include <AppContext.hpp>
#include <Camera.hpp>
#include <CubeRenderer.hpp>

namespace Camera {

    static glm::mat4 view;
    static glm::mat4 projection;

    static glm::vec3 sceneCenter(0.0f);
    static float sceneDist = 5.0f;
    static float sceneRadius = 5.0f;

    // CAMERA ROTATION
    static float angle = 0.0f;
    static float rotationSpeed = 0.5f;

    // CAMERA ZOOM
    static float zoom = 1.0f;
    static float zoomSpeed = 1.0f;
    // static float minZoom = 0.2f;
    // static float maxZoom = 5.0f;

    void RecalculateBounds() {
        const auto& cubes = CubeRenderer::GetCubes();
        if (cubes.empty()) {
            sceneCenter = glm::vec3(0, 0, 0);
            sceneDist = 5.0f;
            sceneRadius = 5.0f;
            return;
        }

        glm::vec3 min(FLT_MAX), max(-FLT_MAX);

        for (const auto& cube : cubes) {
            float scale = cube.scale * CubeRenderer::GetGlobalScale();
            min = glm::min(min, cube.position - glm::vec3(scale));
            max = glm::max(max, cube.position + glm::vec3(scale));
        }

        sceneCenter = 0.5f * (min + max);

        float extent = glm::max(max.x - min.x, max.y - min.y) * 0.5f;

        float fov = 45.0f;
        sceneDist = extent / tan(glm::radians(fov * 0.5f));
        sceneRadius = sceneDist * 1.3f;
    }

    void Init() {
        RecalculateBounds();

        projection = glm::perspective(
            glm::radians(45.0f),
            float(Application::GetWindowWidth()) / Application::GetWindowHeight(),
            0.1f, 1000.0f
        );
    }

    void Update() {
        // CAMERA ORBITS AROUND THE CENTER
        float deltaTime = 1.0f / 60.0f;
        angle += rotationSpeed * deltaTime;

        float dist = sceneRadius / zoom;

        glm::vec3 camPos;
        camPos.x = sceneCenter.x + dist * cos(glm::radians(angle));
        camPos.y = sceneCenter.y + dist * sin(glm::radians(angle));
        camPos.z = sceneCenter.z + (sceneDist * 0.5f) / zoom;

        view = glm::lookAt(camPos, sceneCenter, glm::vec3(0, 0, 1));
    }

    // ACCESSOR METHODS
    const glm::mat4& GetView() { return view; }
    const glm::mat4& GetProjection() { return projection; }

    float& GetRotationSpeed() { return rotationSpeed; }
    float& GetZoom() { return zoom; }

}
