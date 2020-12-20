#include "Processor.hpp"

void Processor::loadLutFromFile(image::Path path) {
    std::ifstream is;
    is.open(path);
    if (!is.is_open()) {
        return; // error: not found
    }
    image::luts::CubeFile cubeFile;
    is >> cubeFile;
    lut = cubeFile.lut();
    interp.load(lut);
    lutLoaded = true;
}
