#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#include <AppContext.hpp>

namespace Camera {

    void RecalculateBounds();

    void Init();

    void Update(float deltaTime);

    // ACCESSOR METHODS
    const glm::mat4& GetView();
    const glm::mat4& GetProjection();

    float& GetRotationSpeed();

    float& GetZoom();
    float& GetMinZoom();
    float& GetMaxZoom();
    float& GetTargetZoom();

}

#endif