#pragma once

#include <image/Color.hpp>
#include <image/filters/Concepts.hpp>
#include <image/luts/Lattice3D.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>

namespace image::filters {

    template <class Interpolator, class T>
    struct Lut {
        luts::Lattice3D lattice { 32 };
        Interpolator interp;

        constexpr Lut() noexcept {
            lattice.loadIdentity();
            interp.load(lattice);
        }

        constexpr void setLattice(luts::Lattice3D l) noexcept {
            lattice = l;
            interp.load(lattice);
        }

        constexpr ColorRGB<T> applyToColor(const ColorRGB<T> &color) const noexcept {
            if (lattice.size == 0) {
                return color;
            }
            return sRgbToLinear(interp.map(linearToSRgb(color)));
        }
    };

}
