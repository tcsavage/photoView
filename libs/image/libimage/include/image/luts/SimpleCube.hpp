#pragma once

#include <array>

#include <glm/glm.hpp>

#include <image/CoreTypes.hpp>
#include <image/Color.hpp>
#include <image/luts/Barycentric.hpp>

namespace image::luts {

    struct SimpleCube {
        std::array<ColorRGB<F32>, 8> inVerts;
        std::array<ColorRGB<F32>, 8> outVerts;

        ColorRGB<F32> map(ColorRGB<F32> color) const;

        void dump() const noexcept;
    };

}
