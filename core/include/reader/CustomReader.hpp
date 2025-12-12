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

#include <Point.hpp>

namespace CustomReader {

    void GetPointData(
        const std::string& filepath, 
        std::unique_ptr<std::vector<Data::Point>>& points,
        uint32_t decimationStep = 1
    );

}