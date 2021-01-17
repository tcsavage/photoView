#pragma once

#include <image/Color.hpp>
#include <image/Precalculator.hpp>
#include <image/filters/Concepts.hpp>
#include <image/luts/Lattice3D.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>

namespace image::filters {

    template <class Interpolator, class T, bool Precalculate = true>
    struct Lut {
        luts::Lattice3D lattice;
        Interpolator interp;
        Precalculator<U8, T> precalc;

        constexpr Lut() noexcept {
            update();
        }

        constexpr void update() noexcept {
            if constexpr(Precalculate) {
                precalc.buildTable([this](const ColorRGB<U8> &color) {
                    return interp.map(conv<T>(color));
                });
            }
        }

        constexpr void setLattice(luts::Lattice3D l) noexcept {
            lattice = l;
            interp.load(lattice);
            update();
        }

        constexpr ColorRGB<T> applyToColor(const ColorRGB<T> &color) const noexcept {
            if (lattice.size == 0) {
                return color;
            }
            if constexpr(Precalculate) {
                return precalc.map(conv<U8, T>(color));
            } else {
                return interp.map(color);
            }
        }
    };

}
