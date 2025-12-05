#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#include <AppContext.hpp>

namespace Camera {

    void RecalculateBounds();

    void Init(Application::AppContext* appContext);

    void Update();

    // ACCESSOR METHODS
    const glm::mat4& GetView();
    const glm::mat4& GetProjection();

    float& GetRotationSpeed();
    float& GetZoom();

}

#endif