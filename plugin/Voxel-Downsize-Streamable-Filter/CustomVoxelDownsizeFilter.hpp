#pragma once

#include <unordered_set>
#include <cstdint>
#include <string>

#include <pdal/Filter.hpp>
#include <pdal/Streamable.hpp>
#include <pdal/PointRef.hpp>
#include <pdal/util/ProgramArgs.hpp>

using namespace pdal;

class CustomVoxelDownsizeFilter : public Filter, public Streamable {
    public:
        CustomVoxelDownsizeFilter() = default;
        std::string getName() const override;

    private:
        void addArgs(ProgramArgs& args) override;
        void ready(PointTableRef table) override;
        bool processOne(PointRef& point) override;
        void done(PointTableRef table) override;

    private:
        double   m_cell = 1.0;
        double   m_invCell = 1.0;
        uint64_t m_expectedPointCount = 0;

        std::unordered_set<uint64_t> m_seen;

    private:
        static inline uint64_t packKey(int64_t x, int64_t y, int64_t z) {
            constexpr uint64_t MASK = (1ULL << 21) - 1;
            
            return  ((uint64_t)(x & MASK) << 42) |
                    ((uint64_t)(y & MASK) << 21) |
                    ((uint64_t)(z & MASK));
        }
};
