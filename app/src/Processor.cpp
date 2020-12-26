#include "Processor.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

#include <OpenImageIO/imageio.h>

image::Expected<void, LutLoadFailure> Processor::loadLutFromFile(image::Path path) {
    std::ifstream is;
    is.open(path);
    if (!is.is_open()) {
        return image::Unexpected(LutLoadFailure(path, "file cound not be opened"));
    }
    image::luts::CubeFile cubeFile;
    is >> cubeFile;
    lut = cubeFile.lut();
    interp.load(lut);
    lutLoaded = true;
    update();
    return image::success;
}

image::Expected<void, ImageLoadFailure> Processor::loadImageFromFile(image::Path path) {
    OIIO::ImageSpec spec;
    auto iin = OIIO::ImageInput::create(path.string());
    if (!iin->open(path, spec, spec)) {
        return image::Unexpected(ImageLoadFailure(path, OIIO::geterror()));
    }
    image::Shape shape {
        static_cast<std::size_t>(spec.width),
        static_cast<std::size_t>(spec.height)
    };
    // Only re-create image buffers if shape is different.
    if (originalImage.shape() != shape) {
        originalImage = image::NDArray<image::ColorRGB<image::U8>>(shape);
        image = image::NDArray<image::ColorRGB<image::U8>>(shape);
    }
    // Assumes a packed, interleaved, RGB layout.
    auto rawArray = originalImage.reinterpret<image::U8>();
    iin->read_image(OIIO::TypeDesc::UINT8, originalImage.data());
    imageWidth = spec.width;
    imageHeight = spec.height;
    iin->close();
    update();
    return image::success;
}

namespace {
    template <class T>
    std::ostream &operator<<(std::ostream &output, const image::ColorRGB<T> &vec) {
        output << "[";
        for (int i = 0; i < 3; i++) {
            if (i > 0) {
                output << ", ";
            }
            output << vec[i];
        }
        return output << "]";
    }
}

void Processor::update() {
    if (lutLoaded) {
        std::cerr << "Applying LUT to original\n";
        std::transform(originalImage.begin(), originalImage.end(), image.begin(), [this](const image::ColorRGB<image::U8> &color) {
            auto out = interp.map(color);
            // std::cerr << "Mapped " << color << " to " << out << "\n";
            return out;
        });
    } else {
        std::cerr << "Coppying original as-is\n";
        std::copy(originalImage.begin(), originalImage.end(), image.begin());
    }
    imageChanged(*this);
}

Processor::Processor(QObject *parent) : QObject(parent) {}
