#pragma once

#include <type_traits>

#include <image/Color.hpp>
#include <image/ImageSpec.hpp>
#include <image/NDArray.hpp>

namespace image {

    template <ChannelsSpec Channels>
    constexpr Shape dimsForChannels(std::size_t width, std::size_t height) {
        if constexpr (Channels::numChannels() > 0) {
            return Shape { Channels::numChannels(), width, height };
        } else {
            return Shape { width, height };
        }
    }

    /**
     * @brief Container for in-memory pixel data.
     *
     * An interleaved pixel layout is assumed.
     *
     * @tparam T The color component type
     * @tparam Channels The channel specification for this image. Defaults to RGB
     */
    template <class T, ChannelsSpec Channels = RGB>
    struct ImageBuf {
        NDArray<T> pixelArray;
        ImageSize size;

        T *data() noexcept { return reinterpret_cast<T *>(pixelArray.data()); }
        const T *data() const noexcept { return reinterpret_cast<const T *>(pixelArray.data()); }

        std::size_t width() const noexcept { return size.x; }
        std::size_t height() const noexcept { return size.y; }

        ColorRGB<T> *begin() noexcept { return pixelArray.begin(); }
        const ColorRGB<T> *begin() const noexcept { return pixelArray.begin(); }

        ColorRGB<T> *end() noexcept { return pixelArray.end(); }
        const ColorRGB<T> *end() const noexcept { return pixelArray.end(); }

        template <typename = typename std::enable_if_t<Channels::numChannels() != 0>>
        T &at(std::size_t c, std::size_t x, std::size_t y) noexcept {
            return pixelArray.at(c, x, y);
        }

        template <typename = typename std::enable_if_t<Channels::numChannels() != 0>>
        const T &at(std::size_t c, std::size_t x, std::size_t y) const noexcept {
            return pixelArray.at(c, x, y);
        }

        typename Channels::VectorType<T> &at(std::size_t x, std::size_t y) noexcept {
            return *reinterpret_cast<typename Channels::VectorType<T> *>(&at(0, x, y));
        }

        const typename Channels::VectorType<T> &at(std::size_t x, std::size_t y) const noexcept {
            return *reinterpret_cast<const typename Channels::VectorType<T> *>(&at(0, x, y));
        }

        ImageBuf() noexcept : pixelArray(Shape { Channels::numChannels(), 0, 0 }) {}
        ImageBuf(std::size_t width, std::size_t height) noexcept
          : pixelArray(Shape { Channels::numChannels(), width, height })
          , size(width, height) {}
        ImageBuf(const ImageSize &s) noexcept : pixelArray(Shape { Channels::numChannels(), s.x, s.y }), size(s) {}
        explicit ImageBuf(const NDArray<ColorRGB<T>> &array) noexcept : pixelArray(array), size(array.shape().at(1), array.shape().at(2)) {}
        explicit ImageBuf(NDArray<ColorRGB<T>> &&array) noexcept : pixelArray(std::move(array)), size(pixelArray.shape().at(1), pixelArray.shape().at(2)) {}
    };

}
