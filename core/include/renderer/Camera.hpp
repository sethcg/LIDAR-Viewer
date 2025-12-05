#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#include <AppContext.hpp>

namespace Camera {

    void Init(Application::AppContext* appContext);

    void Update(Application::AppContext* appContext);

    const glm::mat4& GetView();
    const glm::mat4& GetProjection();

}

#endif