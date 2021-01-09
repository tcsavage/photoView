#pragma once

#include <concepts>

#include <image/Color.hpp>
#include <image/CoreTypes.hpp>

namespace image::filters {

    template <class F, class T>
    concept FilterImpl = requires(F filter, const ColorRGB<T> &color) {
        { filter.applyToColor(color) } noexcept -> std::same_as<ColorRGB<T>>;
    };

}
