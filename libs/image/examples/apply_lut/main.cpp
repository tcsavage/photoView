#include <iostream>
#include <fstream>

#include <OpenImageIO/imageio.h>

#include <image/CoreTypes.hpp>
#include <image/ImageBuf.hpp>
#include <image/IO.hpp>
#include <image/luts/CubeFile.hpp>
#include <image/luts/FastInterpolator.hpp>
#include <image/luts/Lut.hpp>
#include <image/luts/StrengthModifier.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>

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

    std::cerr << "Loading LUT\n";

    std::ifstream lutStream;
    lutStream.open(lutPath);
    luts::CubeFile cube;
    lutStream >> cube;
    luts::Lut lut = cube.lut();
    std::cerr << "Loaded LUT: " << lutPath << "\n";

    luts::FastInterpolator<
        luts::StrengthModifier<
            luts::FastInterpolator<
                luts::TetrahedralInterpolator,
                U8, F32
            >
        >,
        U8, U8
    > interp;
    {
        Timer timer { "Loading LUT into interpolator" };
        interp.load(lut);
    }
    {
        Timer timer { "Updating LUT strength factor" };
        interp.factor = 0.5;
        interp.buildTable();
    }

    std::cerr << "Reading input image\n";
    auto imageResult = image::readImageBufFromFile<image::U8>(inputImagePath);
    auto image = *imageResult;

    std::cerr << "Applying LUT\n";
    {
        Timer timer { "Applying LUT" };
        std::transform(image.begin(), image.end(), image.begin(), [&] (const ColorRGB<U8> &color) { return interp.map(color); });
    }
    std::cerr << "Finished applying LUT\n";

    std::cerr << "Writing output\n";
    image::writeImageBufToFile(outputImagePath, image);

    std::cerr << "Done.\n";

    return 0;
}
