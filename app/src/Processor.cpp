#include "Processor.hpp"

#include <algorithm>
#include <execution>
#include <fstream>
#include <iostream>

#include <OpenImageIO/imageio.h>

#include <image/IO.hpp>
#include <image/luts/Hald.hpp>

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

void Processor::loadHald(std::size_t lutSize) {
    proc.setOriginal(image::generateHald<image::F32, image::U8>(lutSize));
    proc.process();
}

image::Expected<void, LutLoadFailure> Processor::loadLutFromFile(image::Path path) {
    std::ifstream is;
    is.open(path);
    if (!is.is_open()) {
        return image::Unexpected(LutLoadFailure(path, "file cound not be opened"));
    }
    image::luts::CubeFile cubeFile;
    is >> cubeFile;
    proc.filter.withImpl([&](auto impl) { impl->setLut(cubeFile.lut()); });
    proc.filter.update();
    lutLoaded = true;
    proc.process();
    return image::success;
}

image::Expected<void, ImageLoadFailure> Processor::loadImageFromFile(image::Path path) {
    auto result = image::readImageBufFromFile<image::F32>(path);
    if (result.hasError()) {
        return image::Unexpected(ImageLoadFailure(path, result.error().reason));
    }
    proc.setOriginal(*result);
    proc.process();
    return image::success;
}

image::Expected<void, ImageExportFailure> Processor::exportImageToFile(image::Path path) const {
    auto result = image::writeImageBufToFile<image::U8>(path, proc.viewportOutput);
    if (result.hasError()) {
        return image::Unexpected(ImageExportFailure(path, result.error().reason));
    }
    return image::success;
}

void Processor::setProcessingEnabled(bool processingEnabled) {
    proc.setProcessingEnabled(processingEnabled);
    proc.process();
}

void Processor::setLutStrengthFactor(image::F32 factor) {
    proc.filter.setStrength(factor);
    proc.filter.update();
    proc.process();
}
