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

#include <CubeRenderer.hpp>
#include <CustomLazHeader.hpp>

namespace CustomReader {

    CustomLazHeader* GetLazHeader(const std::string& filepath);

    void ReadPointData(
        const std::string& filepath,
        CustomLazHeader& header,
        CubeRenderer& cubeRenderer,
        uint64_t decimationStep = 2
    );

}