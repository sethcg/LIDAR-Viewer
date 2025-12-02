#ifndef CUSTOM_READER_H
#define CUSTOM_READER_H

#include <iostream>
#include <vector>

namespace CustomReader {

    struct Point {
        double x;
        double y;
        double z;
        uint16_t intensity;
    };

    void GetPointData(
        const std::string& filename, 
        std::vector<Point>* points, 
        uint32_t decimationStep = 1
    );

}


#endif