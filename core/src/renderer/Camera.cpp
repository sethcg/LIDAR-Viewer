#include <glm/gtc/matrix_transform.hpp>

#include <AppContext.hpp>
#include <Camera.hpp>
#include <CubeRenderer.hpp>

namespace Camera {

    static glm::mat4 view;
    static glm::mat4 projection;

    // INIT EMPTY SCENE CAMERA
    void Init(Application::AppContext* appContext) {
        view = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        projection = glm::perspective(
            glm::radians(45.0f),
            float(appContext->width) / appContext->height,
            0.1f, 1000.0f
        );
    }

    // UPDATE CAMERA BASED ON THE CUBES
    void Update(Application::AppContext* appContext) {
        const auto& cubes = CubeRenderer::GetCubes();
        if (cubes.empty()) {
            Init(appContext);
            return;
        }

        glm::vec3 min(FLT_MAX), max(-FLT_MAX);
        for (const auto& cube : cubes) {
            float scale = cube.scale * appContext->globalState->scale;
            min = glm::min(min, cube.position - glm::vec3(scale));
            max = glm::max(max, cube.position + glm::vec3(scale));
        }

        glm::vec3 center = 0.5f * (min + max);
        float extent = glm::max(max.x - min.x, max.y - min.y) * 0.5f;

        float fov = 45.0f;
        float dist = extent / tan(glm::radians(fov * 0.5f));

        glm::vec3 camPos = center + glm::vec3(0, -dist * 1.3f, dist * 0.9f);

        view = glm::lookAt(camPos, center, glm::vec3(0, 0, 1));

        projection = glm::perspective(
            glm::radians(fov),
            float(appContext->width) / appContext->height,
            0.1f, 5000.0f
        );
    }

    // ACCESSOR METHODS
    const glm::mat4& GetView() { return view; }
    const glm::mat4& GetProjection() { return projection; }

}
