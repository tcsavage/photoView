#pragma once

#include <cmath>

#include <image/Color.hpp>
#include <image/Precalculator.hpp>
#include <image/filters/Concepts.hpp>
#include <image/luts/Lut.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>

namespace image::filters {

    template <class T>
    constexpr T evToScale(T evs) {
       return std::exp2(evs);
    }

    template <class T>
    struct Exposure {
        T exposureFactor { 1.0 };

        constexpr void setExposure(T evs = 1.0) { exposureFactor = evToScale(evs); }

        constexpr ColorRGB<T> applyToColor(const ColorRGB<T> &color) const noexcept {
            return color * exposureFactor;
        }
    };

}
