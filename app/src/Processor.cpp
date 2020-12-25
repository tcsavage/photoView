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
    return image::success;
}

image::Expected<void, ImageLoadFailure> Processor::loadImageFromFile(image::Path path) {
    OIIO::ImageSpec spec;
    auto iin = OIIO::ImageInput::create(path.string());
    if (!iin->open(path, spec, spec)) {
        return image::Unexpected(ImageLoadFailure(path, OIIO::geterror()));
    }
    image = image::NDArray<image::U8>(image::Shape {
        static_cast<std::size_t>(spec.nchannels),
        static_cast<std::size_t>(spec.width),
        static_cast<std::size_t>(spec.height)
    });
    iin->read_image(OIIO::TypeDesc::UINT8, image.data());
    imageWidth = spec.width;
    imageHeight = spec.height;
    iin->close();
    return image::success;
}
