#pragma once

#include <fstream>

#include <OpenImageIO/imagebuf.h>

#include <image/CoreTypes.hpp>
#include <image/luts/CubeFile.hpp>
#include <image/luts/FastInterpolator.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>

struct Processor {
    image::luts::FastInterpolator<image::U8, image::luts::TetrahedralInterpolator> interp;
    image::luts::LUT lut;
    OIIO::ImageBuf image;
    bool lutLoaded { false };

    bool loadLutFromFile(image::Path path);
    bool loadImageFromFile(image::Path path);
};


