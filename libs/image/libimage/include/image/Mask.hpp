#pragma once

#include <image/CoreTypes.hpp>
#include <image/NDArray.hpp>

namespace image {

    struct Mask {
        NDArray<F32> pixelArray;

        inline F32 *data() noexcept { return reinterpret_cast<F32*>(pixelArray.data()); }
        inline const F32 *data() const noexcept { return reinterpret_cast<const F32*>(pixelArray.data()); }

        inline memory::Size width() const noexcept { return pixelArray.shape().at(0); }
        inline memory::Size height() const noexcept { return pixelArray.shape().at(1); }

        explicit Mask(memory::Size width, memory::Size height) noexcept : pixelArray(Shape{ width, height }) {}
    };

    Mask makeTestMask(memory::Size width, memory::Size height) noexcept;

}
