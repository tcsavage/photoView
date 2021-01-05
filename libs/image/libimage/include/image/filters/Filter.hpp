#pragma once

#include <image/Color.hpp>
#include <image/Precalculator.hpp>
#include <image/filters/Filter.hpp>

namespace image::filters {

    template <FilterImpl Impl, class T, bool Precalculate = true>
    struct Filter {
        Impl impl;
        F32 strength { 1.0f };
        Precalculator<U8, F32> precalc;

        constexpr void setStrength(F32 s) noexcept {
            strength = s;
            if constexpr(Precalculate) {
                precalc.buildTable([this](const ColorRGB<U8> &color) {
                    auto fcolor = conv<F32>(color);
                    return mix(strength, impl.applyToColor(fcolor), fcolor);
                });
            }
        }

        constexpr ColorRGB<T> applyToColor(const ColorRGB<T> &color) const noexcept {
            if constexpr(Precalculate) {
                return precalc.map(conv<U8>(color));
            } else {
                return mix(strength, impl.applyToColor(color), color);
            }
        }
    };

}
