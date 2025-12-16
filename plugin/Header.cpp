
#include <iostream>

#include <lazperf/Extractor.hpp>

#include "Header.hpp"

void Header::fill(const char *buffer, size_t buffersize) {
    lazperf::LeExtractor stream(buffer, buffersize);

    stream.get(magic, 4);
    stream >> fileSourceId >> globalEncoding;

    char guidBuf[Uuid::size()];
    stream.get(guidBuf, Uuid::size());
    projectGuid.unpack(guidBuf);

    stream >> versionMajor >> versionMinor;
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

    for (uint32_t& pbr : legacyPointsByReturn)
        stream >> pbr;

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

std::vector<std::string> Header::validate(uint64_t fileSize) const {
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
        errors.push_back("Unsupported LAS input point format: " + Utils::toString((int)pointFormat()) + ".");
    }
    return errors;
}