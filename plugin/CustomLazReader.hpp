#pragma once

#include <string>
#include <iostream>

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
            m_numPoints = 0;
            m_index = 0;

            reader = nullptr;

            m_xScale = 1.0;
            m_yScale = 1.0;
            m_zScale = 1.0;

            m_xOffset = 0.0;
            m_yOffset = 0.0;
            m_zOffset = 0.0;
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
        pdal::point_count_t m_numPoints;
        pdal::point_count_t m_index;

        // LAZPERF READER
        lazperf::reader::named_file* reader;

        // SCALING VALUES
        double m_xScale;
        double m_yScale;
        double m_zScale;

        // OFFSET VALUES
        double m_xOffset;
        double m_yOffset;
        double m_zOffset;
};