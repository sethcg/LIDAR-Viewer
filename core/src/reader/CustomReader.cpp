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

#include <Point.hpp>

namespace CustomReader {

    void GetPointData(
        const std::string& filepath, 
        std::unique_ptr<std::vector<Data::Point>>& points,
        uint32_t decimationStep,
        double voxelSize)
    {
        try {
            // ALLOCATE POINTS IF NOT ALREADY
            if (!points) points = std::make_unique<std::vector<Data::Point>>();

            auto start = std::chrono::high_resolution_clock::now();
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "\nREADING FILE: %s", filepath.c_str());

            pdal::StageFactory factory;

            // CREATE CUSTOM LAZ READER
            pdal::Stage* reader = factory.createStage("readers.fastlaz");
            if (!reader) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FAILED TO CREATE LAS/LAZ READER");
                return;
            }
            pdal::Options readerOptions;
            readerOptions.add("filename", filepath);

            reader->setOptions(readerOptions);

            pdal::Stage* lastStage = reader;

            // OPTIONAL DECIMATION FILTER
            // TODO: FIX CUSTOM READER GOES THROUGH ALL POINTS (NO PERFORMANCE GAIN)
            if (decimationStep > 1) {
                pdal::Stage* decimation = factory.createStage("filters.decimation");
                pdal::Options decimationOptions;
                decimationOptions.add("step", decimationStep);
                decimation->setOptions(decimationOptions);
                decimation->setInput(*lastStage);
                lastStage = decimation;
            }

            // OPTIONAL VOXELGRID FILTER
            // THIS REMOVES OVERLAPPING CUBES (SMALL PERFORMANCE LOSS)
            if (voxelSize > 0.0) {
                pdal::Stage* voxel = factory.createStage("filters.voxelcenternearestneighbor");
                pdal::Options voxelOptions;
                voxelOptions.add("cell", voxelSize);
                voxel->setOptions(voxelOptions);
                voxel->setInput(*lastStage);
                lastStage = voxel;
            }

            // EXECUTE PIPELINE
            pdal::PointTable table;
            lastStage->prepare(table);
            pdal::PointViewSet viewSet = lastStage->execute(table);

            size_t totalPoints = 0;
            for (const auto& view : viewSet) totalPoints += view->size();

            points->clear();
            points->resize(totalPoints);
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "TOTAL POINTS: %llu", totalPoints);

            std::vector<uint16_t> intensities;
            intensities.reserve(totalPoints);

            // FILL POINT DATA
            size_t offset = 0;
            for (const auto& view : viewSet) {
                for (pdal::PointId idx = 0; idx < view->size(); ++idx) {
                    Data::Point& point = (*points)[offset];

                    // TODO: READ DIRECTLY TO CUBE (NO POINT DATA "MIDDLE MAN")
                    point.x = view->getFieldAs<double>(pdal::Dimension::Id::X, idx);
                    point.y = view->getFieldAs<double>(pdal::Dimension::Id::Y, idx);
                    point.z = view->getFieldAs<double>(pdal::Dimension::Id::Z, idx);

                    point.intensity = view->getFieldAs<uint16_t>(pdal::Dimension::Id::Intensity, idx);
                    intensities.push_back(point.intensity);

                    ++offset;
                }
            }

            // NORMALIZE INTENSITY (1% – 99% PERCENTILE)
            // TODO: IMPLEMENT CUMULATIVE HISTOGRAM
            std::sort(intensities.begin(), intensities.end());
            uint16_t min_intensity = intensities[static_cast<size_t>(0.01 * intensities.size())];
            uint16_t max_intensity = intensities[static_cast<size_t>(0.99 * intensities.size())];

            for (auto& point : *points) {
                uint16_t clamped = std::min(std::max(point.intensity, min_intensity), max_intensity);
                point.normalized = float(clamped - min_intensity) / float(max_intensity - min_intensity);
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "FINISHED READING IN %ld ms", duration.count());
        }
        catch (const pdal::pdal_error& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PDAL ERROR WHILE PROCESSING FILE %s: %s", filepath.c_str(), e.what());
            points->clear();
            return;
        }
        catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ERROR WHILE PROCESSING FILE %s: %s", filepath.c_str(), e.what());
            points->clear();
            return;
        }
    }

}