#pragma once

#include <concepts>

#include <image/CoreTypes.hpp>
#include <image/Color.hpp>
#include <image/ImageBuf.hpp>
#include <image/NDArray.hpp>

namespace image {

    template <std::unsigned_integral T>
    ImageBuf<T> generateHald(std::size_t sizeClass) {
        std::size_t lutSize = sizeClass * sizeClass;
        std::size_t haldSize = lutSize * sizeClass;
        std::size_t sizeBits = sizeof(T) * 8;
        std::size_t size = 1 << sizeBits;
        std::size_t step = size / lutSize;

        NDArray<ColorRGB<T>> lut { Shape { lutSize, lutSize, lutSize }};
        for (std::size_t b = 0; b < lutSize; ++b) {
            for (std::size_t g = 0; g < lutSize; ++g) {
                for (std::size_t r = 0; r < lutSize; ++r) {
                    lut.at(r, g, b) = ColorRGB<T>(r * step, g * step, b * step);
                }
            }
        }
        auto haldShape = Shape { haldSize, haldSize };
        return ImageBuf<T>{ lut.reshape(haldShape) };
    }

}
