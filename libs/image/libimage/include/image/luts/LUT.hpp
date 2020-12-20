#pragma once

#include <cstddef>

#include <image/CoreTypes.hpp>
#include <image/NDArray.hpp>
#include <image/Color.hpp>

namespace image::luts {
    
    struct LUT {
        LUT() noexcept;
        LUT(std::size_t size) noexcept;
    
        std::size_t size { 0 };
        ColorRGB domainMin { 0.0f, 0.0f, 0.0f };
        ColorRGB domainMax { 1.0f, 1.0f, 1.0f };
        NDArray<ColorRGB> table { Shape{} };
    };

}
