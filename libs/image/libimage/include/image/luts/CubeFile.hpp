#pragma once

#include <istream>
#include <ostream>

#include <image/CoreTypes.hpp>
#include <image/NDArray.hpp>
#include <image/Color.hpp>
#include <image/luts/LUT.hpp>

namespace image::luts {

    struct CubeFile {
        CubeFile() noexcept {}

        CubeFile(LUT &lut) noexcept;
        
        LUT lut() const noexcept;
        
        friend std::istream &operator>>(std::istream &input, CubeFile &data);
        friend std::ostream &operator<<(std::ostream &output, const CubeFile &data);

        std::size_t size { 0 };
        String title { "" };
        ColorRGB domainMin { 0.0f, 0.0f, 0.0f };
        ColorRGB domainMax { 1.0f, 1.0f, 1.0f };
        NDArray<ColorRGB> table { Shape{} };
    };

}
