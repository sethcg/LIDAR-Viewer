#include <pdal/pdal.hpp>
#include <pdal/PipelineManager.hpp>
#include <pdal/PointView.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include <CustomReader.hpp>

int main(int argc, char* argv[]) {

    std::string input = "../assets/autzen.laz";
    std::string output = "../assets/output.txt";

    return CustomReader::ReadWritePointData(input, output);
}