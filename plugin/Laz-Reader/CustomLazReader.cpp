#include <algorithm>
#include <chrono>
#include <string>
#include <iostream>
#include <memory>

#include <pdal/Dimension.hpp>
#include <pdal/PointView.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/Reader.hpp>
#include <pdal/Streamable.hpp>
#include <pdal/util/ProgramArgs.hpp>

#include <lazperf/las.hpp>
#include <lazperf/readers.hpp>

#include <CustomLazReader.hpp>

using namespace pdal;

static StaticPluginInfo const s_info {
    "readers.customlaz",
    "Custom Laz Reader (LAZPERF)",
    "",
    { "laz" }
};

CREATE_SHARED_STAGE(CustomLazReader, s_info)
std::string CustomLazReader::getName() const { return s_info.name; }

void CustomLazReader::addArgs(ProgramArgs& args) {}

void CustomLazReader::initialize() {
    m_reader = new lazperf::reader::named_file(m_filename);
    if (!m_reader) throwError("FAILED TO CREATE LAZPERF READER");
    
    const lazperf::header14& header = m_reader->header();
    m_pointBuffer.resize(header.point_record_length);

    m_xScale = header.scale.x;
    m_yScale = header.scale.y;
    m_zScale = header.scale.z;

    m_xOffset = header.offset.x;
    m_yOffset = header.offset.y;
    m_zOffset = header.offset.z;
}

void CustomLazReader::addDimensions(PointLayoutPtr layout) {
    using namespace Dimension;

    layout->registerDim(Id::X, Type::Double);
    layout->registerDim(Id::Y, Type::Double);
    layout->registerDim(Id::Z, Type::Double);
    layout->registerDim(Id::Intensity, Type::Unsigned16);
}

void CustomLazReader::ready(PointTableRef table) {
    m_numPoints = m_reader->pointCount();
    m_index = 0;
}

point_count_t CustomLazReader::read(PointViewPtr view, point_count_t count) {
    point_count_t remainingPoints = m_numPoints - m_index;
    point_count_t pointsToRead = (std::min)(count, remainingPoints);

    PointRef point(*view, 0);
    for (point_count_t i = 0; i < pointsToRead; ++i) {
        m_reader->readPoint(m_pointBuffer.data());
        
        const char* data = m_pointBuffer.data();
        int32_t x = lazperf::utils::unpack<uint32_t>(data); data += 4;
        int32_t y = lazperf::utils::unpack<uint32_t>(data); data += 4;
        int32_t z = lazperf::utils::unpack<uint32_t>(data); data += 4;
        uint16_t intensity = lazperf::utils::unpack<uint16_t>(data);

        point.setPointId(m_index);
        point.setField(Dimension::Id::X, (x * m_xScale) + m_xOffset);
        point.setField(Dimension::Id::Y, (y * m_yScale) + m_yOffset);
        point.setField(Dimension::Id::Z, (z * m_zScale) + m_zOffset);
        point.setField(Dimension::Id::Intensity, intensity);

        m_index++;
    }
    return pointsToRead;
}

bool CustomLazReader::processOne(PointRef& point) {
    if (m_index >= m_numPoints) return false;

    m_reader->readPoint(m_pointBuffer.data());
    
    const char* data = m_pointBuffer.data();
    int32_t x = static_cast<double>(lazperf::utils::unpack<uint32_t>(data)); data += 4;
    int32_t y = static_cast<double>(lazperf::utils::unpack<uint32_t>(data)); data += 4;
    int32_t z = static_cast<double>(lazperf::utils::unpack<uint32_t>(data)); data += 4;
    uint16_t intensity = lazperf::utils::unpack<uint16_t>(data);

    point.setField(Dimension::Id::X, (x * m_xScale) + m_xOffset);
    point.setField(Dimension::Id::Y, (y * m_yScale) + m_yOffset);
    point.setField(Dimension::Id::Z, (z * m_zScale) + m_zOffset);
    point.setField(Dimension::Id::Intensity, intensity);

    m_index++;

    return true;
}

void CustomLazReader::done(PointTableRef /*table*/) {
    delete m_reader;
    m_reader = nullptr;
}