#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <pdal/StageFactory.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/Options.hpp>
#include <pdal/Dimension.hpp>

#include <Camera.hpp>
#include <Cube.hpp>
#include <CubeRenderer.hpp>

namespace CustomReader {

    void GetPointData(
        const std::string& filepath, 
        Camera& camera,
        CubeRenderer& cubeRenderer,
        double voxelSize = 0.2f,
        uint32_t decimationStep = 1
    );

}