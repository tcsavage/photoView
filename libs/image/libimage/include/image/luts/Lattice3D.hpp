#pragma once

#include <cstddef>

#include <image/CoreTypes.hpp>
#include <image/NDArray.hpp>
#include <image/Color.hpp>

namespace image::luts {
    
    struct Lattice3D {
        Lattice3D() noexcept;
        Lattice3D(std::size_t size) noexcept;
    
        std::size_t size { 0 };
        ColorRGB<F32> domainMin { 0.0f, 0.0f, 0.0f };
        ColorRGB<F32> domainMax { 1.0f, 1.0f, 1.0f };
        NDArray<ColorRGB<F32>> table { Shape{} };
    };

}
