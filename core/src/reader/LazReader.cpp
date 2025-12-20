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
#include <LazReader.hpp>

using namespace pdal;

namespace CustomReader {

    // CONSTUCTOR
    LazReader::LazReader(const std::string& filepath, CubeRenderer* cubeRenderer, uint64_t decimationStep) {
        options.filepath = filepath;
        options.cubeRenderer = cubeRenderer;
        options.decimationStep = decimationStep;
        options.header = GetLazHeader(filepath);
    }

    std::shared_ptr<LazHeader> LazReader::GetLazHeader(const std::string& filepath) {
        std::ifstream inputStream(filepath, std::ios::binary);
        if (!(inputStream.is_open() && inputStream.good())) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FAILED TO OPEN INPUT LAZ FILE STREAM");
            return nullptr;
        }

        char headerBuffer[LazHeader::Size14];
        inputStream.read(headerBuffer, LazHeader::Size14);
        if (inputStream.gcount() < static_cast<std::streamsize>(LazHeader::Size12)) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FILE SIZE INSUFFICIENT");
            return nullptr;
        }

        auto header = std::make_shared<LazHeader>();
        header->fill(headerBuffer, LazHeader::Size14);

        uint64_t fileSize = Utils::fileSize(filepath);
        StringList errors = header->validate(fileSize);
        if (!errors.empty()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", errors.front());
        }

        return header;
    }

    void LazReader::ReadPointData() {
        if (!options.header) return;

        auto start = std::chrono::steady_clock::now();

        // CREATE LAS/LAZ READER
        StageFactory factory;
        Stage* lastStage = CreateLazReader(options.filepath, factory);

        // [OPTIONAL] DECIMATION FILTER
        lastStage = AddDecimationFilter(options.decimationStep, lastStage, factory);

        // CREATE FINAL STREAM CALLBACK (FOR POINT PROCESSING)
        std::unique_ptr<pdal::StreamCallbackFilter> callback = CreateStreamCallback(lastStage, factory);

        // CREATE FIXED POINT TABLE
        uint64_t pointCount = options.header->pointCount() / options.decimationStep;
        FixedPointTable table(pointCount);

        // EXECUTE PIPELINE
        callback->prepare(table);
        callback->execute(table);

        auto end = std::chrono::steady_clock::now();
        double seconds = std::chrono::duration<double>(end - start).count();
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
            "TOTAL POINTS: %llu, FINISHED READING IN %.4f seconds (%llu pts/sec)",
            pointCount, seconds, static_cast<unsigned long long>(pointCount / seconds));
    }

    Stage* LazReader::CreateLazReader(const std::string& filepath, StageFactory& factory) {
        // CUSTOM LAZ READER [OBSOLETE]: ABOUT 20-25% SLOWER THAN BUILT-IN READER
        // TLDR: STREAMCALLBACK FIXED MASSIVE PERFORMANCE LOSS
        // Stage* reader = factory.createStage("readers.customlaz");
        Stage* reader = factory.createStage("readers.las");
        if (!reader) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FAILED TO CREATE LAS/LAZ READER");
            return nullptr;
        }
        Options readerOptions;
        readerOptions.add("filename", filepath);
        readerOptions.add("threads", std::thread::hardware_concurrency());
        reader->setOptions(readerOptions);
        return reader;
    }

    Stage* LazReader::AddDecimationFilter(uint64_t decimationStep, Stage* lastStage, StageFactory& factory) {
        if (decimationStep > 1) {
            Stage* decimation = factory.createStage("filters.decimation");
            Options decimationOptions;
            decimationOptions.add("step", decimationStep);
            decimation->setOptions(decimationOptions);
            decimation->setInput(*lastStage);
            lastStage = decimation;
        }
        return lastStage;
    }

    std::unique_ptr<StreamCallbackFilter> LazReader::CreateStreamCallback(Stage* lastStage, StageFactory& factory) {
        std::unique_ptr<pdal::StreamCallbackFilter> callbackFilter = std::make_unique<StreamCallbackFilter>();

        std::shared_ptr<LazHeader> header = options.header;
        CubeRenderer* cubeRenderer = options.cubeRenderer;
        glm::vec3 center(
            (options.header->minX + options.header->maxX) / 2.0f,
            (options.header->minY + options.header->maxY) / 2.0f,
            (options.header->minZ + options.header->maxZ) / 2.0f
        );

        callbackFilter->setCallback([header, cubeRenderer, center](PointRef& point) -> bool {
            // POINT POSITION CENTERED AROUND THE (0, 0, 0)
            double x = point.getFieldAs<double>(Dimension::Id::X);
            double y = point.getFieldAs<double>(Dimension::Id::Y);
            double z = point.getFieldAs<double>(Dimension::Id::Z);
            glm::vec3 position = glm::vec3(x, y, z) - center;

            // NORMALIZED COLOR AROUND (0.0 - 1.0) FOR THE GPU SHADERS
            glm::vec3 color = glm::vec3(1.0f);
            if(header->hasColor()) {
                color = glm::vec3(
                    point.getFieldAs<double>(Dimension::Id::Red) / 255.0f,
                    point.getFieldAs<double>(Dimension::Id::Green) / 255.0f,
                    point.getFieldAs<double>(Dimension::Id::Blue) / 255.0f
                );
            }

            // ADD CUBE
            uint16_t intensity = point.getFieldAs<uint16_t>(Dimension::Id::Intensity);
            cubeRenderer->AddCube(position, intensity);

            // TRUE TO KEEP POINT, FALSE TO DISCARD THE POINT
            return true;
        });

        callbackFilter->setInput(*lastStage);
        return callbackFilter;
    }

}
