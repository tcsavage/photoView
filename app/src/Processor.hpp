#pragma once

#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/NDArray.hpp>
#include <image/luts/CubeFile.hpp>
#include <image/luts/FastInterpolator.hpp>
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
    image::luts::FastInterpolator<image::luts::TetrahedralInterpolator, image::U8, image::U8> interp;
    image::luts::Lut lut;
    image::NDArray<image::ColorRGB<image::U8>> originalImage;
    image::NDArray<image::ColorRGB<image::U8>> image;
    image::F32 lutMixFactor { 1.0f };
    int imageWidth;
    int imageHeight;
    bool lutLoaded { false };

    void loadHald(std::size_t lutSize);

    image::Expected<void, LutLoadFailure> loadLutFromFile(image::Path path);
    image::Expected<void, ImageLoadFailure> loadImageFromFile(image::Path path);
    image::Expected<void, ImageExportFailure> exportImageToFile(image::Path path) const;

    void update();
};
