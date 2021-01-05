#pragma once

#include <image/Color.hpp>
#include <image/CoreTypes.hpp>

namespace image::filters {

    template <class F>
    concept FilterImpl = requires(F filter, const ColorRGB<F32> &color) {
        { filter.applyToColor(color) } noexcept -> std::same_as<ColorRGB<F32>>;
    };

}
