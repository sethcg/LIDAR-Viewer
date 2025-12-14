#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <SDL3/SDL.h>
#include <lazperf/las.hpp>
#include <lazperf/readers.hpp>

#include <Cube.hpp>
#include <CubeRenderer.hpp>

namespace CustomLargeFileReader {

    void TestCreateCubeDirect(const std::string& filename, CubeRenderer& cubeRenderer);

}