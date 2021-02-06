#include "Processor.hpp"

#include <algorithm>
#include <iostream>

#include <image/IO.hpp>
#include <image/luts/Hald.hpp>
#include <image/opencl/Manager.hpp>

void Processor::loadHald(std::size_t lutSize) {
    input = image::generateHald<image::F32, image::U8>(lutSize);
    proc.process(input, output);
}

image::Expected<void, LutLoadFailure> Processor::loadLutFromFile(image::Path path) {
    lutFilter->lut.setPath(path);
    auto result = lutFilter->lut.load();
    if (result.hasError()) {
        return image::Unexpected(LutLoadFailure(path, "file cound not be opened"));
    }
    lutFilter->update();
    proc.update();
    proc.process(input, output);
    return image::success;
}

image::Expected<void, ImageLoadFailure> Processor::loadImageFromFile(image::Path path) {
    auto result = image::readImageBufFromFile<image::F32>(path);
    if (result.hasError()) {
        return image::Unexpected(ImageLoadFailure(path, result.error().reason));
    }
    input = *result;
    output = image::ImageBuf<image::U8>(input.width(), input.height());

    input.pixelArray.buffer()->device = image::opencl::Manager::the()->bufferDevice;
    input.pixelArray.buffer()->deviceMalloc();
    input.pixelArray.buffer()->copyHostToDevice();

    output.pixelArray.buffer()->device = image::opencl::Manager::the()->bufferDevice;
    output.pixelArray.buffer()->deviceMalloc();

    proc.process(input, output);
    return image::success;
}

image::Expected<void, ImageExportFailure> Processor::exportImageToFile(image::Path path) const {
    auto result = image::writeImageBufToFile<image::U8>(path, output);
    if (result.hasError()) {
        return image::Unexpected(ImageExportFailure(path, result.error().reason));
    }
    return image::success;
}

void Processor::setProcessingEnabled(bool processingEnabled) {
    proc.look = processingEnabled ? look : identityLook;
    proc.update();
    proc.process(input, output);
}

void Processor::setLutStrengthFactor(image::F32 factor) {
    lutFilter->strength = factor;
    lutFilter->update();
    proc.update();
    proc.process(input, output);
}

void Processor::setExposure(image::F32 exposure) {
    exposureFilter->exposureEvs = exposure;
    exposureFilter->update();
    proc.update();
    proc.process(input, output);
}

Processor::Processor() noexcept
    : identityLook(std::make_shared<image::Look>())
    , look(std::make_shared<image::Look>())
    , proc(look) {
    proc.init();
    lutFilter = look->addFilter(image::LutFilterSpec {}).as<image::LutFilterSpec>();
    exposureFilter = look->addFilter(image::ExposureFilterSpec {}).as<image::ExposureFilterSpec>();
    proc.update();
}
