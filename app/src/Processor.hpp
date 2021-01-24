#pragma once

#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/ImageBuf.hpp>
#include <image/ImageProcessor.hpp>
#include <image/filters/Exposure.hpp>
#include <image/filters/Lut.hpp>
#include <image/luts/CubeFile.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>

struct LutLoadFailure {
    image::Path path;
    image::String reason;
};

struct ImageLoadFailure {
    image::Path path;
    image::String reason;
};

struct ImageExportFailure {
    image::Path path;
    image::String reason;
};

struct Processor {
    image::ImageProcessor<
        image::filters::Exposure<image::F32>,
        image::filters::Lut<image::luts::TetrahedralInterpolator, image::F32>
    > proc;
    bool lutLoaded { false };

    void loadHald(std::size_t lutSize);

    image::Expected<void, LutLoadFailure> loadLutFromFile(image::Path path);
    image::Expected<void, ImageLoadFailure> loadImageFromFile(image::Path path);
    image::Expected<void, ImageExportFailure> exportImageToFile(image::Path path) const;

    void setProcessingEnabled(bool processingEnabled);
    void setLutStrengthFactor(image::F32 factor);
    void setExposure(image::F32 exposure = 0.0f);

    Processor() noexcept;
};
