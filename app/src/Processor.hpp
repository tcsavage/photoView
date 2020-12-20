#pragma once

#include <fstream>

#include <image/CoreTypes.hpp>
#include <image/luts/CubeFile.hpp>
#include <image/luts/FastInterpolator.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>

struct Processor {
    image::luts::FastInterpolator<image::U8, image::luts::TetrahedralInterpolator> interp;
    image::luts::LUT lut;
    bool lutLoaded { false };

    void loadLutFromFile(image::Path path);
};


