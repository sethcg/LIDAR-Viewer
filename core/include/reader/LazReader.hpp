// #pragma once

// #include <chrono>
// #include <cstdint>
// #include <memory>
// #include <string>
// #include <iostream>
// #include <vector>

// #include <SDL3/SDL.h>
// #include <pdal/Dimension.hpp>
// #include <pdal/Options.hpp>
// #include <pdal/PointTable.hpp>
// #include <pdal/PointView.hpp>
// #include <pdal/StageFactory.hpp>
// #include <pdal/Streamable.hpp>
// #include <pdal/filters/StreamCallbackFilter.hpp>

// #include <CubeRenderer.hpp>
// #include <LazHeader.hpp>

// namespace CustomReader {

//     struct ReaderOptions {
//         std::string filepath;
//         std::shared_ptr<LazHeader> header;
//         CubeRenderer* cubeRenderer;
//         uint64_t decimationStep = 1;
//     };

//     std::shared_ptr<LazHeader> GetLazHeader(const std::string& filepath);
    
//     void ReadPointData(const ReaderOptions& options);

//     Stage* CreateLasReader(const std::string& filepath, StageFactory& factory);

//     Stage* AddDecimationFilter(uint64_t decimationStep, Stage* lastStage, StageFactory& factory);

//     std::unique_ptr<StreamCallbackFilter> CreateStreamCallback(const ReaderOptions& options, Stage* lastStage, StageFactory& factory);

// }

#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <iostream>
#include <vector>

#include <SDL3/SDL.h>
#include <pdal/Dimension.hpp>
#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/Streamable.hpp>
#include <pdal/filters/StreamCallbackFilter.hpp>

#include <CubeRenderer.hpp>
#include <LazHeader.hpp>

using namespace pdal;

namespace CustomReader {

    struct ReaderOptions {
        std::string filepath;
        std::shared_ptr<LazHeader> header;
        CubeRenderer* cubeRenderer;
    };

    class LazReader {
        public:
            LazReader(
                const std::string& filepath, 
                CubeRenderer* cubeRenderer
            );

            void ReadPointData();
            
            // ACCESSORS
            inline std::shared_ptr<LazHeader> GetHeader() const { return options.header; }

        private:
            ReaderOptions options;

            std::shared_ptr<LazHeader> GetLazHeader(const std::string& filepath);

            Stage* CreateLazReader(const std::string& filepath, StageFactory& factory);

            Stage* AddDecimationFilter(uint64_t* pointCount, Stage* lastStage, StageFactory& factory);

            std::unique_ptr<StreamCallbackFilter> CreateStreamCallback(Stage* lastStage, StageFactory& factory);
            
    };

}
