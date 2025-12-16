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

        int32_t x = lazperf::utils::unpack<uint32_t>(data); data += 4;
        int32_t y = lazperf::utils::unpack<uint32_t>(data); data += 4;
        int32_t z = lazperf::utils::unpack<uint32_t>(data); data += 4;
        uint16_t intensity = lazperf::utils::unpack<uint16_t>(data);

        point.setPointId(i);
        point.setField(Dimension::Id::X, x * m_xScale + xBase);
        point.setField(Dimension::Id::Y, y * m_yScale + yBase);
        point.setField(Dimension::Id::Z, z * m_zScale + zBase);
        point.setField(Dimension::Id::Intensity, intensity);
    }
    return pointsToRead;
}

void CustomLazReader::done(PointTableRef /*table*/) {
    delete reader;
    reader = nullptr;
}

// point_count_t CustomLazReader::read(PointViewPtr view, point_count_t count) {
//     point_count_t remainingPoints = m_numPoints - m_index;
//     point_count_t pointsToRead = (std::min)(count, remainingPoints);

//     const double xBase = m_xOffset - center_x;
//     const double yBase = m_yOffset - center_y;
//     const double zBase = m_zOffset - center_z;

//     auto start = std::chrono::high_resolution_clock::now();
//     auto decomp_time = std::chrono::microseconds(0);
//     auto process_time = std::chrono::microseconds(0);

//     // std::vector<char> pointBuffer(header->pointSize);
//     for (point_count_t i = 0; i < pointsToRead; ++i) {
//         auto decomp_start = std::chrono::high_resolution_clock::now();
//         reader->readPoint(pointBuffer.data());
//         auto decomp_end = std::chrono::high_resolution_clock::now();
//         decomp_time += std::chrono::duration_cast<std::chrono::microseconds>(decomp_end - decomp_start);
        
//         auto process_start = std::chrono::high_resolution_clock::now();
//         const char* data = pointBuffer.data();
//         const int32_t x = *reinterpret_cast<const int32_t*>(data);
//         const int32_t y = *reinterpret_cast<const int32_t*>(data + 4);
//         const int32_t z = *reinterpret_cast<const int32_t*>(data + 8);
//         const uint16_t intensity = *reinterpret_cast<const uint16_t*>(data + 12);

//         const PointId pid = m_index++;
//         view->setField(Dimension::Id::X, pid, x * m_xScale + xBase);
//         view->setField(Dimension::Id::Y, pid, y * m_yScale + yBase);
//         view->setField(Dimension::Id::Z, pid, z * m_zScale + zBase);
//         view->setField(Dimension::Id::Intensity, pid, intensity);
//         auto process_end = std::chrono::high_resolution_clock::now();
//         process_time += std::chrono::duration_cast<std::chrono::microseconds>(process_end - process_start);
//     }
    
//     auto total = std::chrono::high_resolution_clock::now() - start;
//     std::cout << "Read " << pointsToRead << " points:\n"
//               << "  Decompression: " << decomp_time.count() / 1000.0 << "ms\n"
//               << "  Processing: " << process_time.count() / 1000.0 << "ms\n"
//               << "  Total: " << std::chrono::duration_cast<std::chrono::milliseconds>(total).count() << "ms\n";
    
//     return pointsToRead;
// }