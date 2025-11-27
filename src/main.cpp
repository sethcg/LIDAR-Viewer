#include <pdal/pdal.hpp>
#include <pdal/PipelineManager.hpp>
#include <pdal/PointView.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {

    std::string inputFile = "../assets/autzen.laz";
    std::string outputFile = "../assets/output.txt";

    try {

        // CREATE PIPELINE MANAGER
        pdal::PipelineManager manager;
        pdal::StageFactory factory;

        // CREATE LAS/LAZ READER
        pdal::Stage* reader = factory.createStage("readers.las");

        // SET OPTIONS FOR READER
        pdal::Options readerOptions;
        readerOptions.add("filename", inputFile);
        reader->setOptions(readerOptions);

        manager.addStage(reader);

        // EXECUTE READER PIPELINE
        manager.execute();

        // GET THE POINT VIEWS
        pdal::PointViewSet views = manager.views();

        // OPEN OUTPUT TEXT FILE
        std::ofstream ofs(outputFile);
        if (!ofs) {
            std::cerr << "Cannot open output file: " << outputFile << std::endl;
            return 1;
        }

        // WRITE CSV HEADER
        ofs << "X,Y,Z,Intensity,ReturnNumber,NumberOfReturns\n";

        // ITERATE ALL POINTS
        for (pdal::PointViewPtr view : views)  {
            // GET FIRST 100 POINTS
            for (pdal::PointId i = 0; i < 100; ++i) {
            // for (pdal::PointId i = 0; i < view->size(); ++i) {
                double x = view->getFieldAs<double>(pdal::Dimension::Id::X, i);
                double y = view->getFieldAs<double>(pdal::Dimension::Id::Y, i);
                double z = view->getFieldAs<double>(pdal::Dimension::Id::Z, i);
                uint16_t intensity = view->getFieldAs<uint16_t>(pdal::Dimension::Id::Intensity, i);
                uint8_t returnNum = view->getFieldAs<uint8_t>(pdal::Dimension::Id::ReturnNumber, i);
                uint8_t numReturns = view->getFieldAs<uint8_t>(pdal::Dimension::Id::NumberOfReturns, i);

                ofs << x << "," << y << "," << z << "," << intensity
                    << "," << static_cast<int>(returnNum)
                    << "," << static_cast<int>(numReturns) << "\n";
            }
        }

        std::cout << "Finished writing points to " << outputFile << std::endl;
    }
    catch (const pdal::pdal_error& e)
    {
        std::cerr << "PDAL error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}