#pragma once

#include <array>
#include <unordered_set>

#include <pdal/util/Uuid.hpp>

using namespace pdal;

struct Header {

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
    uint16_t headerSize {};            // Same as VLR offset
    uint16_t& vlrOffset {headerSize};  // Synonym
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

    // FILL HEADER CONTENTS FROM BUFFER
    void fill(const char *buffer, size_t buffersize);

    // VALIDATE HEADER CONTENTS
    std::vector<std::string> validate(uint64_t fileSize) const;

    inline int size() const { 
        return 
            versionMinor >= 4 ? Header::Size14 : 
            versionMinor == 3 ? Header::Size13 : 
            Header::Size12; 
    }

    inline int ebCount() const {
        int base = baseCount();
        return (base ? pointSize - base : 0);
    }

    inline int pointFormat() const {
        return pointFormatBits & Header::FormatMask; 
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
        return pointFormatBits & Header::CompressionMask; 
    }

    inline uint64_t pointCount() const { 
        return versionMinor >= 4 ? ePointCount : legacyPointCount; 
    }

    inline int maxReturnCount() const { 
        return versionMinor >= 4 ? Header::ReturnCount : Header::LegacyReturnCount; 
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

};