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

#include <Camera.hpp>
#include <CubeRenderer.hpp>
#include <CustomLazHeader.hpp>

using namespace pdal;

namespace CustomReader {

    CustomLazHeader* GetLazHeader(const std::string& filepath) {
        std::ifstream inputStream(filepath, std::ios::binary);
        if(!(inputStream.is_open() && inputStream.good())) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FAILED TO OPEN INPUT LAZ FILE STREAM");
            return nullptr;
        }
        inputStream.seekg(0);

        char headerBuffer[CustomLazHeader::Size14];
        inputStream.read(headerBuffer, CustomLazHeader::Size14);
        if (inputStream.gcount() < (std::streamsize)CustomLazHeader::Size12) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FILE SIZE INSUFFICIENT");
            return nullptr;
        }

        CustomLazHeader* header = new CustomLazHeader();
        header->fill(headerBuffer, CustomLazHeader::Size14);
        
        // VALIDATE LAZ/LAS HEADER
        uint64_t fileSize = Utils::fileSize(filepath);
        StringList errors = header->validate(fileSize);
        if (errors.size()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", errors.front());
        }

        return header;
    }

    void ReadPointData(
        const std::string& filepath,
        CustomLazHeader& header,
        CubeRenderer& cubeRenderer,
        uint64_t decimationStep)
    {
        auto start = std::chrono::high_resolution_clock::now();

        StageFactory factory;

        // CUSTOM LAZ READER [OBSOLETE]: ABOUT 20-25% SLOWER THAN BUILT-IN READER
        // TLDR: STREAMCALLBACK FIXED MASSIVE PERFORMANCE LOSS
        // Stage* reader = factory.createStage("readers.customlaz");

        // CREATE LAS/LAZ READER
        Stage* reader = factory.createStage("readers.las");
        if (!reader) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FAILED TO CREATE LAS/LAZ READER");
            return;
        }
        Options readerOptions;
        readerOptions.add("filename", filepath);
        readerOptions.add("threads", std::thread::hardware_concurrency());
        reader->setOptions(readerOptions);
        Stage* lastStage = reader;

        // OPTIONAL DECIMATION FILTER
        if (decimationStep > 1) {
            Stage* decimation = factory.createStage("filters.decimation");
            Options decimationOptions;
            decimationOptions.add("step", decimationStep);
            decimation->setOptions(decimationOptions);
            decimation->setInput(*lastStage);
            lastStage = decimation;
        }

        // CREATE FINAL STREAM CALLBACK (FOR POINT PROCESSING)
        StreamCallbackFilter callbackFilter;

        glm::vec3 center = glm::vec3(
            (header.minX + header.maxX) / 2.0f, 
            (header.minY + header.maxY) / 2.0f, 
            (header.minZ + header.maxZ) / 2.0f
        );
        callbackFilter.setCallback([&cubeRenderer, &header, center](PointRef& point) -> bool {
            
            // POINT POSITION CENTERED AROUND THE (0, 0, 0)
            double x = point.getFieldAs<double>(Dimension::Id::X);
            double y = point.getFieldAs<double>(Dimension::Id::Y);
            double z = point.getFieldAs<double>(Dimension::Id::Z);
            glm::vec3 position = glm::vec3(x, y, z) - center;

            // NORMALIZED COLOR AROUND (0.0 - 1.0) FOR THE GPU SHADERS
            glm::vec3 color = glm::vec3(1.0f);
            if(header.hasColor()) {
                color = glm::vec3(
                    point.getFieldAs<double>(Dimension::Id::Red) / 255.0f,
                    point.getFieldAs<double>(Dimension::Id::Green) / 255.0f,
                    point.getFieldAs<double>(Dimension::Id::Blue) / 255.0f
                );
            }

            // ADD CUBE
            uint16_t intensity = point.getFieldAs<uint16_t>(Dimension::Id::Intensity);
            cubeRenderer.AddCube(position, intensity);
            
            // TRUE TO KEEP POINT, FALSE TO DISCARD THE POINT
            return true;
        });
        callbackFilter.setInput(*lastStage);

        // CREATE FIXED POINT TABLE
        uint64_t pointCount = header.pointCount() / decimationStep;
        FixedPointTable table(pointCount);

        // EXECUTE PIPELINE
        callbackFilter.prepare(table);
        callbackFilter.execute(table);

        auto end = std::chrono::high_resolution_clock::now();
        double seconds = std::chrono::duration<double>(end - start).count();
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
            "TOTAL POINTS: %llu, FINISHED READING IN %.4f seconds (%llu pts/sec)",
            pointCount, seconds, static_cast<unsigned long long>(pointCount / seconds));
    }

}