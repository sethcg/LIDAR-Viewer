#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
#include <string>

#include <SDL3/SDL.h>
#include <pdal/pdal.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/io/LasReader.hpp>
#include <pdal/filters/DecimationFilter.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>

#include <CustomReader.hpp>
#include <Point.hpp>

namespace CustomReader {

    void GetPointData(
        std::string filepath, 
        std::vector<Data::Point>* points,
        uint32_t decimationStep) 
    {
        if (!points) return;

        auto start = std::chrono::high_resolution_clock::now();
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "READING FILE: %s", filepath.c_str());

        pdal::StageFactory factory;

        // CREATE LAS/LAZ READER
        pdal::Stage* reader = factory.createStage("readers.las");
        pdal::Options readerOptions;
        readerOptions.add("filename", filepath);
        reader->setOptions(readerOptions);

        pdal::Stage* lastStage = reader;

        // APPLY DECIMATION FILTER
        if (decimationStep > 1) {
            pdal::Stage* decimation = factory.createStage("filters.decimation");
            pdal::Options decimationOptions;
            decimationOptions.add("step", decimationStep);
            decimation->setOptions(decimationOptions);
            decimation->setInput(*lastStage);
            lastStage = decimation;
        }

        // APPLY OUTLIER FILTER (ONLY WORKS ON SPATIAL DATA "XYZ")
        // pdal::Stage* outlier = factory.createStage("filters.outlier");
        // pdal::Options outlierOptions;
        // outlierOptions.add("method", "statistical");
        // outlierOptions.add("mean_k", 8);
        // outlierOptions.add("multiplier", 2.5);
        // outlier->setOptions(outlierOptions);
        // outlier->setInput(*lastStage);
        // lastStage = outlier;

        // EXECUTE PIPELINE
        pdal::PointTable table;
        lastStage->prepare(table);
        pdal::PointViewSet viewSet = lastStage->execute(table);

        // CALCULATE TOTAL POINT COUNT
        size_t totalPoints = 0;
        for (const auto& view : viewSet) totalPoints += view->size();

        points->clear();
        points->resize(totalPoints);

        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "TOTAL NUMBER OF POINTS: %llu", totalPoints);

        double sumX = 0, sumY = 0, sumZ = 0;
       
        std::vector<uint16_t> intensities;
        intensities.reserve(points->size());

        // WRITE DIRECTLY USING INDEX
        size_t offset = 0;
        for (const auto& view : viewSet) {
            for (pdal::PointId idx = 0; idx < view->size(); ++idx) {
                Data::Point& point = (*points)[offset];

                point.x = view->getFieldAs<double>(pdal::Dimension::Id::X, idx);
                point.y = view->getFieldAs<double>(pdal::Dimension::Id::Y, idx);
                point.z = view->getFieldAs<double>(pdal::Dimension::Id::Z, idx);

                point.r = view->getFieldAs<uint8_t>(pdal::Dimension::Id::Red, idx);
                point.g = view->getFieldAs<uint8_t>(pdal::Dimension::Id::Green, idx);
                point.b = view->getFieldAs<uint8_t>(pdal::Dimension::Id::Blue, idx);

                point.intensity = view->getFieldAs<uint16_t>(pdal::Dimension::Id::Intensity, idx);
                
                // STORE INTENSITY VALUE FOR PERCENTILE NORMALIZING
                intensities.push_back(point.intensity);

                sumX += point.x;
                sumY += point.y;
                sumZ += point.z;

                ++offset;
            }
        }

        // SORT TO COMPUTE PERCENTILES
        std::sort(intensities.begin(), intensities.end());
        
        // CALCULATE ROBUST MIN/MAX USING PERCENTILES
        uint16_t min_intensity = intensities[static_cast<size_t>(0.01 * intensities.size())];
        uint16_t max_intensity = intensities[static_cast<size_t>(0.99 * intensities.size())];

        // CENTER POINTS AROUND 0
        double meanX = sumX / totalPoints;
        double meanY = sumY / totalPoints;
        double meanZ = sumZ / totalPoints;
        
        for (Data::Point& point : *points) {
            point.x -= meanX;
            point.y -= meanY;
            point.z -= meanZ;

            // APPLY ROBUST MIN/MAX TO NORMALIZE
            uint16_t clamped = std::min(std::max(point.intensity, min_intensity), max_intensity);
            point.normalized = float(clamped - min_intensity) / float(max_intensity - min_intensity);
        }

        // for (size_t i = 0; i < std::min(size_t(10), points->size()); ++i) {
        //     SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, 
        //         "RED %f: GREEN %f: BLUE %f:",
        //         (*points)[i].r, 
        //         (*points)[i].g, 
        //         (*points)[i].b
        //     );
        // }

        // for (size_t i = 0; i < std::min(size_t(10), points->size()); ++i) {
        //     SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, 
        //         "POINT %zu: original=%u, clamped=%u, normalized=%f", 
        //         i, 
        //         (*points)[i].intensity, 
        //         std::min(std::max((*points)[i].intensity, min_intensity), max_intensity), 
        //         (*points)[i].normalized
        //     );
        // }
        // SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "MIN INTENSITY: %f", float(min_intensity));
        // SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "MAX INTENSITY: %f", float(max_intensity));

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "DURATION FOR %s: %ld ms\n", filepath.c_str(), duration.count());
    }

}
