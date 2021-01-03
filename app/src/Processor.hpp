#pragma once

#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/ImageBuf.hpp>
#include <image/luts/CubeFile.hpp>
#include <image/luts/FastInterpolator.hpp>
#include <image/luts/StrengthModifier.hpp>
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
    image::luts::FastInterpolator<image::luts::StrengthModifier<image::luts::FastInterpolator<image::luts::TetrahedralInterpolator, image::U8, image::F32>>, image::U8, image::U8> interp;
    image::luts::Lut lut;
    image::ImageBuf<image::U8> originalImage;
    image::ImageBuf<image::U8> image;
    int imageWidth;
    int imageHeight;
    bool lutLoaded { false };

    void loadHald(std::size_t lutSize);

    image::Expected<void, LutLoadFailure> loadLutFromFile(image::Path path);
    image::Expected<void, ImageLoadFailure> loadImageFromFile(image::Path path);
    image::Expected<void, ImageExportFailure> exportImageToFile(image::Path path) const;

    void setLutStrengthFactor(image::F32 factor);

    void update();
};
