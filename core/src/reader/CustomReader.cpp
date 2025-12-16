#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <thread>

#include <SDL3/SDL.h>
#include <pdal/StageFactory.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/Options.hpp>
#include <pdal/Dimension.hpp>

#include <Camera.hpp>
#include <Cube.hpp>
#include <CubeRenderer.hpp>

using namespace pdal;

namespace CustomReader {

    void GetPointData(
        const std::string& filepath, 
        Camera& camera,
        CubeRenderer& cubeRenderer,
        double voxelSize,
        uint32_t decimationStep)
    {
        try {
            auto start = std::chrono::high_resolution_clock::now();

            StageFactory factory;

            // CREATE CUSTOM LAZ READER
            // Stage* reader = factory.createStage("readers.las");
            Stage* reader = factory.createStage("readers.fastlaz");
            if (!reader) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FAILED TO CREATE LAS/LAZ READER");
                return;
            }
            Options readerOptions;
            readerOptions.add("filename", filepath);
            // readerOptions.add("threads", std::thread::hardware_concurrency());

            reader->setOptions(readerOptions);

            Stage* lastStage = reader;

            // OPTIONAL DECIMATION FILTER
            // TODO: FIX CUSTOM READER GOES THROUGH ALL POINTS (NO PERFORMANCE GAIN)
            if (decimationStep > 1) {
                Stage* decimation = factory.createStage("filters.decimation");
                Options decimationOptions;
                decimationOptions.add("step", decimationStep);
                decimation->setOptions(decimationOptions);
                decimation->setInput(*lastStage);
                lastStage = decimation;
            }

            // OPTIONAL VOXELGRID FILTER
            // THIS REMOVES OVERLAPPING CUBES (SMALL PERFORMANCE LOSS)
            if (voxelSize > 0.0) {
                Stage* voxel = factory.createStage("filters.voxelcenternearestneighbor");
                Options voxelOptions;
                voxelOptions.add("cell", voxelSize);
                voxel->setOptions(voxelOptions);
                voxel->setInput(*lastStage);
                lastStage = voxel;
            }

            // EXECUTE PIPELINE
            PointTable table;
            lastStage->prepare(table);
            PointViewSet viewSet = lastStage->execute(table);

            // RETRIEVE METADATA
            MetadataNode metadata = reader->getMetadata();
            double minx = metadata.findChild("minx").value<double>();
            double miny = metadata.findChild("miny").value<double>();
            double minz = metadata.findChild("minz").value<double>();
            glm::vec3 minDistance = glm::vec3(minx, miny, minz);

            double maxx = metadata.findChild("maxx").value<double>();
            double maxy = metadata.findChild("maxy").value<double>();
            double maxz = metadata.findChild("maxz").value<double>();
            glm::vec3 maxDistance = glm::vec3(maxx, maxy, maxz);

            // UPDATE CAMERA BOUNDING BOX
            float radius = glm::length(maxDistance - minDistance) * 0.5f;
            camera.UpdateBounds(glm::vec3(0.0f), radius);

            // GET POINT COUNT
            uint64_t pointCount = 0;
            for (const auto& view : viewSet) pointCount += view->size();

            // MAX NUMBER OF BINS (65535 "uint16_t" SIZE)
            // LOWER VALUE IS FASTER, BUT LESS PRECISE
            const size_t BIN_COUNT = 65535;
            std::vector<size_t> histogram(BIN_COUNT, 0);
            
            std::vector<uint16_t> intensities(pointCount);    
            uint16_t min_intensity = UINT16_MAX;
            uint16_t max_intensity = 0;

            // FILL POINT DATA
            uint64_t offset = 0;
            cubeRenderer.UpdateBufferSize(pointCount);
            for (const PointViewPtr& view : viewSet) {
                for (uint64_t index = 0; index < pointCount; ++index) {

                    glm::vec3 position = glm::vec3(
                        view->getFieldAs<double>(Dimension::Id::X, index),
                        view->getFieldAs<double>(Dimension::Id::Y, index),
                        view->getFieldAs<double>(Dimension::Id::Z, index)
                    );
                    cubeRenderer.AddCube(index, position);

                    uint16_t intensity = view->getFieldAs<uint16_t>(Dimension::Id::Intensity, index);
                    intensities[index] = intensity;

                    // BUILD COLOR HISTOGRAM
                    min_intensity = std::min(min_intensity, intensity);
                    max_intensity = std::max(max_intensity, intensity);
                    histogram[intensity]++;

                    ++offset;
                }
            }
            
            // BUILD CUMULATIVE HISTOGRAM (CUMULATIVE DISTRIBUTION FUNCTION)
            std::vector<size_t> cumulativeHistogram(BIN_COUNT, 0);
            cumulativeHistogram[0] = histogram[0];
            for (size_t i = 1; i < BIN_COUNT; ++i) {
                cumulativeHistogram[i] = cumulativeHistogram[i - 1] + histogram[i];
            }

            for (uint64_t i = 0; i < pointCount; ++i) {
                float normalizedValue = float(cumulativeHistogram[intensities[i]]) / float(pointCount);
                glm::vec3 color = Data::ColorMap(normalizedValue);
                cubeRenderer.UpdateInstanceColor(i, color);
            }

            cubeRenderer.UpdateBuffers();

            auto end = std::chrono::high_resolution_clock::now();
            double seconds = std::chrono::duration<double>(end - start).count();
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                "TOTAL POINTS: %llu, FINISHED READING IN %.4f seconds (%llu pts/sec)",
                pointCount, seconds, static_cast<unsigned long long>(pointCount / seconds));
        }
        catch (const pdal_error& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PDAL ERROR WHILE PROCESSING FILE %s: %s", filepath.c_str(), e.what());
            return;
        }
        catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ERROR WHILE PROCESSING FILE %s: %s", filepath.c_str(), e.what());
            return;
        }
    }

}