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

struct Processor {
    image::luts::FastInterpolator<image::U8, image::luts::TetrahedralInterpolator> interp;
    image::luts::LUT lut;
    image::NDArray<image::U8> image;
    int imageWidth;
    int imageHeight;
    bool lutLoaded { false };

    image::Expected<void, LutLoadFailure> loadLutFromFile(image::Path path);
    image::Expected<void, ImageLoadFailure> loadImageFromFile(image::Path path);
};
