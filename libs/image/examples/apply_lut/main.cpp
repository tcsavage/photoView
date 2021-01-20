#include <iostream>
#include <fstream>

#include <OpenImageIO/imageio.h>

#include <image/CoreTypes.hpp>
#include <image/ImageBuf.hpp>
#include <image/ImageProcessor.hpp>
#include <image/IO.hpp>
#include <image/filters/Filter.hpp>
#include <image/filters/Lut.hpp>
#include <image/luts/CubeFile.hpp>
#include <image/luts/Lattice3D.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>
#include <image/opencl/Manager.hpp>

using namespace image;

class Timer {
public:
    Timer(std::string name) : name_(name), start_(std::chrono::steady_clock::now()) {}

    ~Timer() {
        auto end = std::chrono::steady_clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
        std::cerr << "Timer " << name_ << " took " << dt.count() << " ms\n";
    }
private:
    std::string name_;
    std::chrono::time_point<std::chrono::steady_clock> start_;
};

int main(int argc, const char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " RAW_PHOTO_PATH 3D_CUBE_LUT_PATH OUTPUT_PATH\n";
    }
    String inputImagePath = argv[1];
    String lutPath = argv[2];
    String outputImagePath = argv[3];

    opencl::Manager oclMan;

    ImageProcessor<
        filters::Lut<luts::TetrahedralInterpolator, F32, false>
    > proc;

    {
        Timer timer { "Image Processor - Init" };
        proc.init();
    }

    std::cerr << "Loading LUT\n";

    std::ifstream lutStream;
    lutStream.open(lutPath);
    luts::CubeFile cube;
    lutStream >> cube;
    std::cerr << "Loaded Cube: " << lutPath << "\n";
    {
        Timer timer { "Loading LUT into interpolator" };
        auto &f = proc.getFilter<filters::Lut<luts::TetrahedralInterpolator, F32, false>>();
        f.impl.setLattice(cube.lattice());
        f.update();
    }
    {
        Timer timer { "Updating LUT strength factor" };
        auto &f = proc.getFilter<filters::Lut<luts::TetrahedralInterpolator, F32, false>>();
        f.setStrength(0.5);
        f.update();
    }
    {
        Timer timer { "Updating precalculated filter output" };
        proc.update();
    }

    std::cerr << "Reading input image\n";
    auto imageResult = readImageBufFromFile<F32>(inputImagePath);
    proc.setInput(*imageResult);

    std::cerr << "Applying LUT\n";
    {
        Timer timer { "Applying LUT" };
        proc.process();
    }
    std::cerr << "Finished applying LUT\n";

    std::cerr << "Writing output\n";
    writeImageBufToFile<U8>(outputImagePath, proc.output);

    std::cerr << "Done.\n";

    return 0;
}
