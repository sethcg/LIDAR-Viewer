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

#include <Point.hpp>
#include <CubeRenderer.hpp>

// DEVELOPER NOTE:
// EXPLORATORY TESTING AREA FOR CUSTOM IMPLEMENTED LAZ READER

namespace CustomLargeFileReader {

    static lazperf::reader::named_file* reader;
    static uint64_t pointCount;
    static uint16_t pointSize;

    static glm::vec3 scale = glm::vec3(1.0f);
    static glm::vec3 offset = glm::vec3(0.0f);
    static glm::vec3 center = glm::vec3(0.0f);

    inline void Initialize(const std::string& filename) {
        reader = new lazperf::reader::named_file(filename);
        if (!reader) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "FAILED TO CREATE LAZPERF READER");
        
        pointCount = reader->pointCount();

        const lazperf::header14& header = reader->header();
        pointSize = header.point_record_length;

        scale = glm::vec3(header.scale.x, header.scale.y, header.scale.z);
        offset = glm::vec3(header.offset.x, header.offset.y, header.offset.z);
        center = glm::vec3(
            (header.minx + header.maxx) / 2.0, 
            (header.miny + header.maxy) / 2.0, 
            (header.minz + header.maxz) / 2.0
        );
    }

    void TestCreateCubeDirect(const std::string& filename) {
        auto start = std::chrono::high_resolution_clock::now();
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "\nREADING FILE: %s", filename.c_str());

        // SETUP READER
        Initialize(filename);

        // GET RENDERER VARIABLES
        std::vector<Data::Cube> &cubes = CubeRenderer::GetCubes();
        size_t &instanceCount = CubeRenderer::GetInstanceCount();
        std::vector<glm::mat4> &instanceModels = CubeRenderer::GetInstanceModels();
        std::vector<glm::vec3> &instanceColors = CubeRenderer::GetInstanceColors();

        // UPDATE CUBE GPU BUFFER SIZE
        CubeRenderer::UpdateBufferSize(pointCount);

        cubes.reserve(pointCount);

        // MAX NUMBER OF BINS (65535 "uint16_t" MAX VALUE)
        // LOWER VALUE IS FASTER, BUT LESS PRECISE
        const size_t BIN_COUNT = 65535;
        std::vector<size_t> histogram(BIN_COUNT, 0);
        
        uint16_t min_intensity = UINT16_MAX;
        uint16_t max_intensity = 0;

        std::unique_ptr<char[]> pointbuffer(new char[pointSize]);
        for (uint64_t i = 0; i < pointCount; ++i) {
            reader->readPoint(pointbuffer.get());
            lazperf::las::point14 point(pointbuffer.get());

            glm::vec3 position = glm::vec3(
                double((point.x() * scale.x + offset.x) - center.x),
                double((point.y() * scale.y + offset.y) - center.y),
                double((point.z() * scale.z + offset.z) - center.z)
            );      
            cubes.emplace_back(position, point.intensity());

            // BUILD COLOR HISTOGRAM
            min_intensity = std::min(min_intensity, point.intensity());
            max_intensity = std::max(max_intensity, point.intensity());
            histogram[point.intensity()]++;
        }

        // BUILD CUMULATIVE HISTOGRAM (CUMULATIVE DISTRIBUTION FUNCTION)
        std::vector<size_t> cumulativeHistogram(BIN_COUNT, 0);
        cumulativeHistogram[0] = histogram[0];
        for (size_t i = 1; i < BIN_COUNT; ++i) {
            cumulativeHistogram[i] = cumulativeHistogram[i - 1] + histogram[i];
        }

        for (uint64_t i = 0; i < pointCount; ++i) {
            // APPLY COLOR CHANGES TO CUBE
            float normalizedValue = float(cumulativeHistogram[cubes[i].intensity]) / float(pointCount);
            cubes[i].color = Data::ColorMap(glm::clamp(normalizedValue, 0.0f, 1.0f));

            // DIRECTLY ADD INSTANCE COLOR/MODEL DATA TO GPU BUFFERS
            instanceColors[i] = cubes[i].color;
            instanceModels[i] = glm::translate(glm::mat4(1.0f), cubes[i].position);
            instanceCount++;
        }
        CubeRenderer::UpdateBufferData();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "FINISHED READING IN %ld ms", duration.count());
    }

}