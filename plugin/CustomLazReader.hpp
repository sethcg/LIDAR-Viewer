#pragma once

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

using namespace pdal;

class CustomLazReader : public Reader {
    public:
        CustomLazReader() {
            reader = nullptr;
        };
        std::string getName() const;

    private:
        // PDAL READER OVERRIDES
        virtual void addArgs(ProgramArgs& args);
        virtual void initialize();
        virtual void addDimensions(PointLayoutPtr layout);
        virtual void ready(PointTableRef table);
        virtual point_count_t read(PointViewPtr view, point_count_t num);
        virtual void done(PointTableRef table);

    private:
        pdal::point_count_t m_numPoints = 0;
        pdal::point_count_t m_index = 0;

        // LAZPERF VARIABLES
        lazperf::reader::named_file* reader;
        std::vector<char> pointBuffer;

        // SCALING VALUES
        double m_xScale = 1.0;
        double m_yScale = 1.0;
        double m_zScale = 1.0;

        // OFFSET VALUES
        double m_xOffset = 0.0;
        double m_yOffset = 0.0;
        double m_zOffset = 0.0;

        // CENTER VALUES
        double center_x = 0.0;
        double center_y = 0.0;
        double center_z = 0.0;
};