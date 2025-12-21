#include <algorithm>
#include <cmath>
#include <cstdint>
#include <unordered_set>
#include <string>

#include <pdal/Filter.hpp>
#include <pdal/Streamable.hpp>
#include <pdal/PointRef.hpp>
#include <pdal/util/ProgramArgs.hpp>

#include <CustomVoxelDownsizeFilter.hpp>

using namespace pdal;

static StaticPluginInfo const s_info {
    "filters.voxeldownsizestream",
    "Streamable voxel grid downsampling filter (1 point per cell)",
    ""
};

CREATE_SHARED_STAGE(CustomVoxelDownsizeFilter, s_info)

std::string CustomVoxelDownsizeFilter::getName() const { return s_info.name; }

void CustomVoxelDownsizeFilter::addArgs(ProgramArgs& args) {
    args.add("cell", "Voxel cell size", m_cell, 1.0);
    args.add("expected_point_count", "Estimated input point count (used for reserve heuristic)", m_expectedPointCount, 0ULL);
}

void CustomVoxelDownsizeFilter::ready(PointTableRef) {
    m_seen.clear();
    m_invCell = 1.0 / m_cell;
    if (m_expectedPointCount > 0) {
        m_seen.reserve(static_cast<size_t>(m_expectedPointCount));
    }
}

bool CustomVoxelDownsizeFilter::processOne(PointRef& point) {
    double x = point.getFieldAs<double>(Dimension::Id::X);
    double y = point.getFieldAs<double>(Dimension::Id::Y);
    double z = point.getFieldAs<double>(Dimension::Id::Z);

    int64_t ix = static_cast<int64_t>(x * m_invCell);
    int64_t iy = static_cast<int64_t>(y * m_invCell);
    int64_t iz = static_cast<int64_t>(z * m_invCell);

    uint64_t key = packKey(ix, iy, iz);
    
    return m_seen.insert(key).second;
}

void CustomVoxelDownsizeFilter::done(PointTableRef) {
    m_seen.clear();
}
