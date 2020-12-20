#pragma once

#include <array>

#include <glm/glm.hpp>

#include <image/CoreTypes.hpp>
#include <image/Color.hpp>
#include <image/luts/Barycentric.hpp>

namespace image::luts {

    struct SimpleCube {
        std::array<ColorRGB, 8> inVerts;
        std::array<ColorRGB, 8> outVerts;

        ColorRGB map(ColorRGB color) const;

        void dump() const noexcept;
    };

}
