#pragma once

#include <image/Color.hpp>
#include <image/Precalculator.hpp>
#include <image/filters/Filter.hpp>

namespace image::filters {

    template <class T, FilterImpl<T> Impl, bool Precalculate = true>
    struct Filter {
        Impl impl;
        F32 strength { 1.0f };
        Precalculator<U8, T> precalc;

        constexpr Filter() noexcept {
            update();
        }

        constexpr void update() noexcept {
            if constexpr(Precalculate) {
                precalc.buildTable([this](const ColorRGB<U8> &color) {
                    auto fcolor = conv<T>(color);
                    return mix(strength, impl.applyToColor(fcolor), fcolor);
                });
            }
        }

        constexpr void setStrength(F32 s) noexcept {
            strength = s;
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
