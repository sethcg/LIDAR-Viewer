#pragma once

#include <array>
#include <iostream>
#include <unordered_set>

#include <pdal/util/Uuid.hpp>
#include <pdal/util/Extractor.hpp>

using namespace pdal;

struct LazHeader {

    static const int LegacyReturnCount {5};
    static const int ReturnCount {15};
    static const int Size {375};
    static const int Size12 {227};
    static const int Size13 {235};
    static const int Size14 {375};
    static const int FormatMask {0xF};
    static const int CompressionMask {0x80};

    std::string magic { "LASF" };
    uint16_t fileSourceId {};
    uint16_t globalEncoding {};
    Uuid projectGuid;
    uint8_t versionMajor {1};
    uint8_t versionMinor {2};
    std::string systemId { "PDAL" };
    std::string softwareId;
    uint16_t creationDoy;
    uint16_t creationYear;
    uint16_t headerSize {};            // SAME AS VLR OFFSET
    uint16_t& vlrOffset {headerSize};  // SYNONYM
    uint32_t pointOffset {};
    uint32_t vlrCount {};
    uint8_t pointFormatBits {};
    uint16_t pointSize {};
    uint32_t legacyPointCount {};
    std::array<uint32_t, LegacyReturnCount> legacyPointsByReturn;
    
    // SCALE
    double scaleX;
    double scaleY;
    double scaleZ;

    // OFFSET
    double offsetX;
    double offsetY;
    double offsetZ;

    // BOUNDING BOX
    double minX;
    double minY;
    double minZ;
    double maxX;
    double maxY;
    double maxZ;

    uint64_t waveOffset {};
    uint64_t evlrOffset {};
    uint32_t evlrCount {};
    uint64_t ePointCount {};
    std::array<uint64_t, ReturnCount> ePointsByReturn;

    inline int size() const { 
        return 
            versionMinor >= 4 ? Size14 : 
            versionMinor == 3 ? Size13 : 
            Size12; 
    }

    inline int ebCount() const {
        int base = baseCount();
        return (base ? pointSize - base : 0);
    }

    inline int pointFormat() const {
        return pointFormatBits & FormatMask; 
    }

    inline bool pointFormatSupported() const {
        switch (pointFormat()){
            case 0:
            case 1:
            case 2:
            case 3:
            case 6:
            case 7:
            case 8:
                return true;
            default:
                return false;
        }
    }

    inline int baseCount() const {
        switch (pointFormat()) {
            case 0:
                return 20;
            case 1:
                return 28;
            case 2:
                return 26;
            case 3:
                return 34;
            case 6:
                return 30;
            case 7:
                return 36;
            case 8:
                return 38;
            default:
                return 0;
        }
    }
    
    inline bool dataCompressed() const { 
        return pointFormatBits & CompressionMask; 
    }

    inline uint64_t pointCount() const { 
        return versionMinor >= 4 ? ePointCount : legacyPointCount; 
    }

    inline int maxReturnCount() const { 
        return versionMinor >= 4 ? ReturnCount : LegacyReturnCount; 
    }

    inline bool versionAtLeast(int major, int minor) const { 
        return versionMinor >= minor; 
    }

    inline bool has14PointFormat() const { 
        return pointFormat() > 5; 
    }

    inline bool hasTime() const { 
        return pointFormat() != 2;
    }

    inline bool hasWave() const {
        int format = pointFormat();
        static const std::unordered_set<int> waveFormats = {4, 5, 9, 10};
        return waveFormats.find(format) != waveFormats.end();
    }

    inline bool hasColor() const {
        static const std::unordered_set<int> colorFormats = {2, 3, 5, 7, 8, 10};
        int format = pointFormat();
        return colorFormats.find(format) != colorFormats.end();
    }

    inline bool hasInfrared() const { 
        return pointFormat() == 8; 
    }

    inline void fill(const char *buffer, size_t buffersize) {
        LeExtractor stream(buffer, buffersize);

        stream.get(magic, 4);
        stream >> fileSourceId;
        stream >> globalEncoding;

        char guidBuf[Uuid::size()];
        stream.get(guidBuf, Uuid::size());
        projectGuid.unpack(guidBuf);

        stream >> versionMajor;
        stream >> versionMinor;
        stream.get(systemId, 32);
        stream.get(softwareId, 32);
        stream >> creationDoy;
        stream >> creationYear;
        stream >> headerSize;
        stream >> pointOffset;
        stream >> vlrCount;
        stream >> pointFormatBits;
        stream >> pointSize;
        stream >> legacyPointCount;

        for (uint32_t& pbr : legacyPointsByReturn) {
            stream >> pbr;
        }

        stream >> scaleX >> scaleY >> scaleZ;
        stream >> offsetX >> offsetY >> offsetZ;
        stream >> maxX >> minX >> maxY >> minY >> maxZ >> minZ;
        if (versionMinor >= 3) {
            stream >> waveOffset;
            if (versionMinor >= 4) {
                stream >> evlrOffset >> evlrCount >> ePointCount;
                for (uint64_t& pbr : ePointsByReturn) {
                    stream >> pbr;
                }
            }
        }
    }

    inline std::vector<std::string> validate(uint64_t fileSize) {
        std::vector<std::string> errors;
        if (magic != "LASF") {
            errors.push_back("Invalid file signature. Was expecting 'LASF', Check the first four bytes of the file.");
        }
        if (!dataCompressed() && (pointOffset > fileSize)) {
            errors.push_back("Invalid point offset - exceeds file size.");
        }
        if (!dataCompressed() && (pointOffset + pointCount() * pointSize > fileSize)) {
            errors.push_back("Invalid point count: " + std::to_string(pointCount()) + ". Number of points too large for file size.");
        }
        if (vlrOffset > fileSize) {
            errors.push_back("Invalid VLR offset - exceeds file size.");
        }
        if (!pointFormatSupported()) {
            errors.push_back("Unsupported LAS input point format: " + pdal::Utils::toString((int)pointFormat()) + ".");
        }
        return errors;
    }

};