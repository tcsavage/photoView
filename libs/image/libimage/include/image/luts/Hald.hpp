#pragma once

#include <concepts>

#include <image/CoreTypes.hpp>
#include <image/Color.hpp>
#include <image/NDArray.hpp>

namespace image {

    template <std::unsigned_integral T>
    NDArray<ColorRGB<T>> generateHald(std::size_t lutSize) {
        std::size_t sizeBits = sizeof(T) * 8;
        std::size_t size = 1 << sizeBits;
        std::size_t step = size / lutSize;

        auto haldSize = lutSize * sizeBits;
        NDArray<ColorRGB<T>> lut { Shape { lutSize, lutSize, lutSize }};
        for (std::size_t b = 0; b < lutSize; ++b) {
            for (std::size_t g = 0; g < lutSize; ++g) {
                for (std::size_t r = 0; r < lutSize; ++r) {
                    auto shape = Shape { r, g, b };
                    lut.at(shape) = ColorRGB<T>(r * step, g * step, b * step);
                }
            }
        }
        return lut.reshape(Shape { haldSize, haldSize });
    }

}
