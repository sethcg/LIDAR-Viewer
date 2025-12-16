#pragma once

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

using namespace pdal;

class CustomLazReader : public Reader, public Streamable {
    public:
        CustomLazReader() {
            m_reader = nullptr;
        };
        std::string getName() const;

    private:
        // PDAL READER OVERRIDES
        virtual void addArgs(ProgramArgs& args);
        virtual void initialize();
        virtual void addDimensions(PointLayoutPtr layout);
        virtual void ready(PointTableRef table);
        virtual point_count_t read(PointViewPtr view, point_count_t num);
        virtual bool processOne(PointRef& point);
        virtual void done(PointTableRef table);

    private:
        pdal::point_count_t m_numPoints = 0;
        pdal::point_count_t m_index = 0;

        // LAZPERF VARIABLES
        lazperf::reader::named_file* m_reader;
        std::vector<char> m_pointBuffer;

        // SCALING VALUES
        double m_xScale = 1.0;
        double m_yScale = 1.0;
        double m_zScale = 1.0;

        // OFFSET VALUES
        double m_xOffset = 0.0;
        double m_yOffset = 0.0;
        double m_zOffset = 0.0;

        // CENTER/OFFSET BASE VALUES
        double m_xBase = 0.0;
        double m_yBase = 0.0;
        double m_zBase = 0.0;
};