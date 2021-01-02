#pragma once

#include <image/Color.hpp>
#include <image/NDArray.hpp>

namespace image {

    /**
     * @brief Container for in-memory pixel data.
     * 
     * An interleaved RGB pixel format is assumed. No other formats are supported at this time.
     * 
     * @tparam T The color component type
     */
    template <class T>
    struct ImageBuf {
        NDArray<ColorRGB<T>> pixelArray;

        T *data() noexcept { return reinterpret_cast<T*>(pixelArray.data()); }
        const T *data() const noexcept { return reinterpret_cast<const T*>(pixelArray.data()); }

        std::size_t width() const noexcept { return pixelArray.shape().at(0); }
        std::size_t height() const noexcept { return pixelArray.shape().at(1); }

        ColorRGB<T> *begin() noexcept { return pixelArray.begin(); }
        const ColorRGB<T> *begin() const noexcept { return pixelArray.begin(); }

        ColorRGB<T> *end() noexcept { return pixelArray.end(); }
        const ColorRGB<T> *end() const noexcept { return pixelArray.end(); }

        ImageBuf() noexcept : pixelArray(Shape{ 0, 0 }) {}
        ImageBuf(std::size_t width, std::size_t height) noexcept : pixelArray(Shape{ width, height }) {}
        explicit ImageBuf(const NDArray<ColorRGB<T>> &array) noexcept : pixelArray(array) {}
        explicit ImageBuf(NDArray<ColorRGB<T>> &&array) noexcept : pixelArray(std::move(array)) {}
    };

}
