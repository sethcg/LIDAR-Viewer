#ifndef CUSTOM_READER_H
#define CUSTOM_READER_H

#include <vector>

#include <Point.hpp>

namespace CustomReader {

    void GetPointData(
        std::string filepath, 
        std::vector<Data::Point>* points,
        uint32_t decimationStep = 1
    );

}

#endif