#include <algorithm>
#include <chrono>
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
    pointBuffer.resize(header.point_record_length);

    m_xScale = header.scale.x;
    m_yScale = header.scale.y;
    m_zScale = header.scale.z;

    m_xOffset = header.offset.x;
    m_yOffset = header.offset.y;
    m_zOffset = header.offset.z;

    center_x = (header.minx + header.maxx) / 2.0;
    center_y = (header.miny + header.maxy) / 2.0;
    center_z = (header.minz + header.maxz) / 2.0;

    // ADD THE CENTERED MINIMUM/MAXIMUM VALUES TO ROOT
    MetadataNode root = getMetadata();
    root.add("minx", header.minx);
    root.add("miny", header.miny);
    root.add("minz", header.minz);
    root.add("maxx", header.maxx);
    root.add("maxy", header.maxy);
    root.add("maxz", header.maxz);
}

void CustomLazReader::addDimensions(PointLayoutPtr layout) {
    using namespace Dimension;

    layout->registerDim(Id::X);
    layout->registerDim(Id::Y);
    layout->registerDim(Id::Z);
    layout->registerDim(Id::Intensity);
}

void CustomLazReader::ready(PointTableRef table) {
    m_numPoints = reader->pointCount();
    m_index = 0;
}

point_count_t CustomLazReader::read(PointViewPtr view, point_count_t count) {
    point_count_t remainingPoints = m_numPoints - m_index;
    point_count_t pointsToRead = (std::min)(count, remainingPoints);

    double xBase = m_xOffset - center_x;
    double yBase = m_yOffset - center_y;
    double zBase = m_zOffset - center_z;

    PointRef point(*view, 0);
    for (point_count_t i = 0; i < pointsToRead; ++i) {
        reader->readPoint(pointBuffer.data());
        const char* data = pointBuffer.data();

        int32_t x = lazperf::utils::unpack<int32_t>(data); data += 4;
        int32_t y = lazperf::utils::unpack<int32_t>(data); data += 4;
        int32_t z = lazperf::utils::unpack<int32_t>(data); data += 4;
        uint16_t intensity = lazperf::utils::unpack<uint16_t>(data);

        point.setPointId(m_index);
        point.setField(Dimension::Id::X, x * m_xScale + xBase);
        point.setField(Dimension::Id::Y, y * m_yScale + yBase);
        point.setField(Dimension::Id::Z, z * m_zScale + zBase);
        point.setField(Dimension::Id::Intensity, intensity);

        m_index++;
    }
    return pointsToRead;
}

void CustomLazReader::done(PointTableRef /*table*/) {
    delete reader;
    reader = nullptr;
}