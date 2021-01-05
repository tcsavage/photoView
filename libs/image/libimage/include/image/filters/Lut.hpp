#pragma once

#include <execution>
#include <ranges>

#include <image/Color.hpp>
#include <image/Precalculator.hpp>
#include <image/filters/Concepts.hpp>
#include <image/luts/Lut.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>

namespace image::filters {

    template <class Interpolator, class T, bool Precalculate = true>
    struct Lut {
        luts::Lut lut;
        Interpolator interp;
        Precalculator<U8, T> precalc;

        constexpr void setLut(luts::Lut l) noexcept {
            lut = l;
            interp.load(lut);
            if constexpr(Precalculate) {
                precalc.buildTable([this](const ColorRGB<U8> &color) {
                    return interp.map(conv<T>(color));
                });
            }
        }

        constexpr ColorRGB<T> applyToColor(const ColorRGB<T> &color) const noexcept {
            if constexpr(Precalculate) {
                return precalc.map(conv<U8, T>(color));
            } else {
                return interp.map(color);
            }
        }
    };

}
