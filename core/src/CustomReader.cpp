#include <iostream>
#include <vector>
#include <string>

#include <pdal/pdal.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/io/LasReader.hpp>
#include <pdal/filters/DecimationFilter.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>

#include <CustomReader.hpp>

namespace CustomReader {

    void GetPointData(
        const std::string& filename, 
        std::vector<Point>* points, 
        uint32_t decimationStep) 
    {
        if (!points) return;

        pdal::StageFactory factory;

        // CREATE LAS/LAZ READER
        pdal::Stage* reader = factory.createStage("readers.las");
        pdal::Options readerOptions;
        readerOptions.add("filename", filename);
        reader->setOptions(readerOptions);

        pdal::Stage* lastStage = reader;

        // APPLY DECIMATION FILTER (READ EVERY N-TH POINT)
        if (decimationStep > 1) {
            pdal::Stage* decimation = factory.createStage("filters.decimation");
            pdal::Options decimationOptions;
            decimationOptions.add("step", decimationStep);
            decimation->setOptions(decimationOptions);
            decimation->setInput(*lastStage);
            lastStage = decimation;
        }

        // PREPARE/EXECUTE PIPELINE
        pdal::PointTable table;
        lastStage->prepare(table);
        pdal::PointViewSet viewSet = lastStage->execute(table);

        // CALCULATE TOTAL POINTS/ALLOCATE MEMORY
        size_t totalPoints = 0;
        for (auto const& view : viewSet) totalPoints += view->size();
        points->resize(totalPoints);

        // EXTRACT POINT DATA AND COMPUTE MEAN
        double sumX = 0, sumY = 0, sumZ = 0;
        size_t offset = 0;
        for (auto const& view : viewSet) {
            for (pdal::PointId idx = 0; idx < view->size(); ++idx, ++offset) {
                Point& point = (*points)[offset];
                point.x = view->getFieldAs<double>(pdal::Dimension::Id::X, idx);
                point.y = view->getFieldAs<double>(pdal::Dimension::Id::Y, idx);
                point.z = view->getFieldAs<double>(pdal::Dimension::Id::Z, idx);
                point.intensity = view->getFieldAs<uint16_t>(pdal::Dimension::Id::Intensity, idx);

                sumX += point.x;
                sumY += point.y;
                sumZ += point.z;
            }
        }

        // COMPUTE MEAN
        double meanX = sumX / totalPoints;
        double meanY = sumY / totalPoints;
        double meanZ = sumZ / totalPoints;

        // CENTER POINTS AROUND 0
        for (auto& point : *points) {
            point.x -= meanX;
            point.y -= meanY;
            point.z -= meanZ;
        }
    }

}
