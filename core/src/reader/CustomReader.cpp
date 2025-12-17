#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <thread>

#include <SDL3/SDL.h>
#include <pdal/Dimension.hpp>
#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/Streamable.hpp>
#include <pdal/filters/StreamCallbackFilter.hpp>

#include <Camera.hpp>
#include <Cube.hpp>
#include <CubeRenderer.hpp>
#include <CustomLazHeader.hpp>

using namespace pdal;

namespace CustomReader {

    CustomLazHeader* GetLazHeader(const std::string& filepath, Camera& camera) {
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

        // RETRIEVE METADATA
        glm::vec3 minDistance = glm::vec3(
            header->minX, 
            header->minY, 
            header->minZ
        );
        glm::vec3 maxDistance = glm::vec3(
            header->maxX, 
            header->maxY, 
            header->maxZ
        );

        // UPDATE CAMERA BOUNDING BOX
        float radius = glm::length(maxDistance - minDistance) * 0.5f;
        camera.UpdateBounds(glm::vec3(0.0f), radius);

        return header;
    }

    void GetPointData(
        const std::string& filepath, 
        Camera& camera,
        CubeRenderer& cubeRenderer,
        double voxelSize,
        uint32_t decimationStep)
    {
        try {
            auto start = std::chrono::high_resolution_clock::now();

            CustomLazHeader* header = GetLazHeader(filepath, camera);

            StageFactory factory;

            // CUSTOM LAZ READER IS OBSOLETE, (ABOUT 20-25% SLOWER THAN BUILT-IN READER)
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

            // OPTIONAL VOXELGRID FILTER
            // BROKEN: CANNOT WORK WITH STREAMABLE READERS
            // if (voxelSize > 0.0) {
            //     Stage* voxel = factory.createStage("filters.voxelcenternearestneighbor");
            //     Options voxelOptions;
            //     voxelOptions.add("cell", voxelSize);
            //     voxel->setOptions(voxelOptions);
            //     voxel->setInput(*lastStage);
            //     lastStage = voxel;
            // }

            // APPLY COLOR INTERPOLATION
            // Stage* colorInterpolation = factory.createStage("filters.colorinterp");
            // Options colorOptions;
            // colorOptions.add("dimension", "Intensity");         // APPLY COLOR BASED ON INTENSITY VALUES
            // colorOptions.add("ramp", "pestel_shades");          // COLOR RAMP TO APPLY
            // // colorOptions.add("clamp", true);
            // colorOptions.add("mad", true);                      // ENABLE MAD (MEDIAN ABSOLUTE DEVIATION)
            // colorOptions.add("mad_multiplier", 1.5);            // MAD MULTIPLIER (CONTROL SENSITIVITY)
            // colorOptions.add("k", 1.0);                         // STANDARD DEVIATION THRESHOLD
            // // colorOptions.add("minimum", header->minZ);
            // // colorOptions.add("maximum", header->maxZ);
            // colorInterpolation->setOptions(colorOptions);
            // colorInterpolation->setInput(*lastStage);
            // lastStage = colorInterpolation;

            // CREATE FINAL STREAM CALLBACK (FOR POINT PROCESSING)
            StreamCallbackFilter callbackFilter;

            glm::vec3 center = glm::vec3(
                (header->minX + header->maxX) / 2.0f, 
                (header->minY + header->maxY) / 2.0f, 
                (header->minZ + header->maxZ) / 2.0f
            );
            callbackFilter.setCallback([&cubeRenderer, &header, center](PointRef& point) -> bool {
                
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
                cubeRenderer.AddCube(position, color, intensity);
                
                // TRUE TO KEEP POINT, FALSE TO DISCARD THE POINT
                return true;
            });
            callbackFilter.setInput(*lastStage);

            // CREATE FIXED POINT TABLE
            uint64_t pointCount = header->pointCount();
            FixedPointTable table(pointCount);

            // UPDATE GPU INSTANCE BUFFER SIZES
            cubeRenderer.Clear();
            cubeRenderer.UpdateBufferSize(pointCount);

            // EXECUTE PIPELINE
            callbackFilter.prepare(table);
            callbackFilter.execute(table);

            // NORMALIZE INTENSITY/COLOR VALUES
            cubeRenderer.NormalizeColors();

            // CALL GPU UPDATE
            cubeRenderer.UpdateBuffers();

            auto end = std::chrono::high_resolution_clock::now();
            double seconds = std::chrono::duration<double>(end - start).count();
            uint64_t pointsRead = pointCount / decimationStep;
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,
                "TOTAL POINTS: %llu, FINISHED READING IN %.4f seconds (%llu pts/sec)",
                pointsRead, seconds, static_cast<unsigned long long>(pointsRead / seconds));
        }
        catch (const pdal_error& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "PDAL ERROR WHILE PROCESSING FILE %s: %s", filepath.c_str(), e.what());
            return;
        }
        catch (const std::exception& e) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "ERROR WHILE PROCESSING FILE %s: %s", filepath.c_str(), e.what());
            return;
        }
    }

}