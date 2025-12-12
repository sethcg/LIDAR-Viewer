#include <string>
#include <iostream>
#include <algorithm>

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

    char pointbuf[256];
    for (point_count_t i = 0; i < pointsToRead; ++i) {
        try {
            reader->readPoint(pointbuf);
            lazperf::las::point14 p(pointbuf);

            view->setField(Dimension::Id::X, numRead, p.x());
            view->setField(Dimension::Id::Y, numRead, p.y());
            view->setField(Dimension::Id::Z, numRead, p.z());
            view->setField(Dimension::Id::Intensity, numRead, p.intensity());
            
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
}