#pragma once

#include <concepts>
#include <iostream>

#include <glm/glm.hpp>

#include <image/CoreTypes.hpp>
#include <image/NDArray.hpp>
#include <image/Color.hpp>
#include <image/luts/Lut.hpp>

namespace image::luts {
    
    template <class Wrapped, std::floating_point F = F32>
    class StrengthModifier : public Wrapped {
    public:
        using InType = Wrapped::InType;
        using OutType = Wrapped::OutType;

        ColorRGB<OutType> map(const ColorRGB<InType> &color) const noexcept {
            return mix(factor, conv<InType, OutType>(color), Wrapped::map(color));
        }

        F factor { 1.0 };
    };

}
