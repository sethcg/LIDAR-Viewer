#include <algorithm>
#include <string>
#include <iostream>
#include <memory>

#include <pdal/PointView.hpp>
#include <pdal/Reader.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/util/ProgramArgs.hpp>
#include <pdal/Dimension.hpp>

#include <lazperf/las.hpp>
#include <lazperf/readers.hpp>

#include <CustomLazReader.hpp>

using namespace pdal;

static StaticPluginInfo const s_info {
    "readers.fastlaz",
    "Custom Laz Reader (LAZPERF)",
    "",
    { "laz" }
};

CREATE_SHARED_STAGE(CustomLazReader, s_info)
std::string CustomLazReader::getName() const { return s_info.name; }

void CustomLazReader::addArgs(ProgramArgs& args) {}

void CustomLazReader::initialize() {
    reader = new lazperf::reader::named_file(m_filename);
    if (!reader) throwError("FAILED TO CREATE LAZPERF READER");
    
    const lazperf::header14& header = reader->header();

    m_xScale = header.scale.x;
    m_yScale = header.scale.y;
    m_zScale = header.scale.z;

    m_xOffset = header.offset.x;
    m_yOffset = header.offset.y;
    m_zOffset = header.offset.z;
}

void CustomLazReader::addDimensions(PointLayoutPtr layout) {
    using namespace Dimension;

    layout->registerDim(Id::X);
    layout->registerDim(Id::Y);
    layout->registerDim(Id::Z);
    layout->registerDim(Id::Intensity);
}

void CustomLazReader::ready(PointTableRef /* table */) {
    m_numPoints = reader->pointCount();
    m_index = 0;
}

point_count_t CustomLazReader::read(PointViewPtr view, point_count_t count) {
    point_count_t numRead = 0;
    point_count_t remainingPoints = m_numPoints - m_index;
    point_count_t pointsToRead = (std::min)(count, remainingPoints);

    const lazperf::header14& header = reader->header();
    double centerX = (header.minx + header.maxx) / 2.0;
    double centerY = (header.miny + header.maxy) / 2.0;
    double centerZ = (header.minz + header.maxz) / 2.0;

    // ALLOCATE DYNAMIC BUFFER SIZE
    uint16_t bufferSize = header.point_record_length;
    std::unique_ptr<char[]> pointbuffer(new char[bufferSize]);

    for (point_count_t i = 0; i < pointsToRead; ++i) {
        try {
            reader->readPoint(pointbuffer.get());
            point = new lazperf::las::point14(pointbuffer.get());

            view->setField<double>(Dimension::Id::X, numRead, (point->x() * m_xScale + m_xOffset) - centerX);
            view->setField<double>(Dimension::Id::Y, numRead, (point->y() * m_yScale + m_yOffset) - centerY);
            view->setField<double>(Dimension::Id::Z, numRead, (point->z() * m_zScale + m_zOffset) - centerZ);
            view->setField<uint16_t>(Dimension::Id::Intensity, numRead, point->intensity());

            // if (numRead == 0) {
            //     std::cout << std::fixed
            //         << "\n\tX=" << (point->x() * m_xScale + m_xOffset) - centerX
            //         << "\n\tY=" << (point->y() * m_yScale + m_yOffset) - centerY
            //         << "\n\tZ=" << (point->z() * m_zScale + m_zOffset) - centerZ
            //     << std::endl;
            // }
            
            numRead++;
            m_index++;
        } catch (const std::exception& e) {
            std::cerr << "ERROR READING POINT " << m_index << ": " << e.what() << std::endl;
            break;
        }
    }
    return numRead;
}

void CustomLazReader::done(PointTableRef /*table*/) {
    if (reader) {
        delete reader;
        reader = nullptr;
    }
    if (point) {
        delete point;
        point = nullptr;
    }
}