#include <iostream>
#include <fstream>

#include <OpenImageIO/imageio.h>

#include <image/CoreTypes.hpp>
#include <image/ImageBuf.hpp>
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

ImageBuf<U8> readImage(Path path) {
    OIIO::ImageSpec spec;
    auto iin = OIIO::ImageInput::create(path.string());
    if (!iin) {
        throw std::exception();
    }
    if (!iin->open(path, spec, spec)) {
        throw std::exception();
    }
    auto imageBuf = image::ImageBuf<image::U8>(spec.width, spec.height);
    // Assumes a packed, interleaved, RGB layout.
    iin->read_image(0, 0, 0, 4, OIIO::TypeDesc::UINT8, imageBuf.data(), OIIO::AutoStride, OIIO::AutoStride, OIIO::AutoStride, nullptr, nullptr);
    return imageBuf;
}

void writeImage(Path path, const ImageBuf<U8> imageBuf) {
    auto iout = OIIO::ImageOutput::create(path.string());
    OIIO::ImageSpec spec(imageBuf.width(), imageBuf.height(), 3, OIIO::TypeDesc::UINT8);
    if (!iout->open(path.string(), spec)) {
        throw std::exception();
    }
    if (!iout->write_image(OIIO::TypeDesc::UINT8, imageBuf.data())) {
        throw std::exception();
    }
    iout->close();
}

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
    auto image = readImage(inputImagePath);

    std::cerr << "Applying LUT\n";
    {
        Timer timer { "Applying LUT" };
        std::transform(image.begin(), image.end(), image.begin(), [&] (const ColorRGB<U8> &color) { return interp.map(color); });
    }
    std::cerr << "Finished applying LUT\n";

    std::cerr << "Writing output\n";
    writeImage(outputImagePath, image);

    std::cerr << "Done.\n";

    return 0;
}
