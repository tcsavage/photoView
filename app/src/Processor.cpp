#include "Processor.hpp"

#include <iostream>

bool Processor::loadLutFromFile(image::Path path) {
    std::ifstream is;
    is.open(path);
    if (!is.is_open()) {
        return false; // error: not found
    }
    image::luts::CubeFile cubeFile;
    is >> cubeFile;
    lut = cubeFile.lut();
    interp.load(lut);
    lutLoaded = true;
    return true;
}

bool Processor::loadImageFromFile(image::Path path) {
    image = OIIO::ImageBuf(image::String(path));
    image.read(0, 0, true);
    return !image.has_error();
}
