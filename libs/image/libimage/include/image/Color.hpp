#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <limits>

#include <glm/glm.hpp>

#include <image/CoreTypes.hpp>
#include <image/TypeName.hpp>

namespace image {

    template <class T>
    struct ColorRGB final : public glm::vec<3, T, glm::defaultp> {
        constexpr ColorRGB() noexcept : glm::vec<3, T, glm::defaultp>(0) {}

        template <class... Args>
        constexpr ColorRGB(Args &&... args) noexcept : glm::vec<3, T, glm::defaultp>(std::forward<Args>(args)...) {}
    };

    template <class Out, class In>
    requires(std::unsigned_integral<In> && std::unsigned_integral<Out>) || (std::floating_point<In> && std::floating_point<Out>)
    constexpr Out conv(const In &in) noexcept { return in; }

    template <std::floating_point Out, std::unsigned_integral In>
    constexpr Out conv(const In &in) noexcept {
        return static_cast<Out>(in) / static_cast<Out>(std::numeric_limits<In>::max());
    }

    template <std::unsigned_integral Out, std::floating_point In>
    constexpr Out conv(const In &in) noexcept {
        return static_cast<Out>(std::clamp(in * static_cast<In>(std::numeric_limits<Out>::max()),
                                           static_cast<In>(0),
                                           static_cast<In>(std::numeric_limits<Out>::max())));
    }

    template <class Out, class In>
    constexpr ColorRGB<Out> conv(const ColorRGB<In> &in) noexcept {
        return ColorRGB<Out> { conv<Out, In>(in.r), conv<Out, In>(in.g), conv<Out, In>(in.b) };
    }

    template <std::floating_point T>
    constexpr T mix(T factor, T a, T invFactor, T b) noexcept {
        return (factor * a) + (invFactor * b);
    }

    template <std::floating_point T>
    constexpr T mix(T factor, T a, T b) noexcept {
        return mix(factor, a, 1 - factor, b);
    }

    template <std::unsigned_integral T, std::floating_point F = F32>
    constexpr T mix(F factor, T a, F invFactor, T b) noexcept {
        return conv<T, F>(mix(factor, conv<F, T>(a), invFactor, conv<F, T>(b)));
    }

    template <std::unsigned_integral T, std::floating_point F = F32>
    constexpr T mix(F factor, T a, T b) noexcept {
        return conv<T, F>(mix(factor, conv<F, T>(a), conv<F, T>(b)));
    }

    template <class T, std::floating_point F = F32>
    constexpr ColorRGB<T> mix(F factor, ColorRGB<T> a, F invFactor, ColorRGB<T> b) noexcept {
        return ColorRGB<T> { mix(factor, a.r, invFactor, b.r),
                             mix(factor, a.g, invFactor, b.g),
                             mix(factor, a.b, invFactor, b.b) };
    }

    template <class T, std::floating_point F = F32>
    constexpr ColorRGB<T> mix(F factor, ColorRGB<T> a, ColorRGB<T> b) noexcept {
        return mix(factor, a, 1 - factor, b);
    }

    template <std::floating_point T>
    constexpr T sRgbToLinear(T in) noexcept {
        if (in <= 0.04045) {
            return in / 12.92;
        } else {
            return glm::pow((in + 0.055) / 1.055, 2.4);
        }
    }

    template <std::floating_point T>
    constexpr T linearToSRgb(T in) noexcept {
        if (in <= 0.0031308) {
            return in * 12.92;
        } else {
            return (1.055 * glm::pow(in, 1.0 / 2.4)) - 0.055;
        }
    }

    template <std::floating_point T>
    constexpr ColorRGB<T> sRgbToLinear(ColorRGB<T> in) noexcept {
        return ColorRGB<T> { sRgbToLinear(in.x), sRgbToLinear(in.y), sRgbToLinear(in.z) };
    }

    template <std::floating_point T>
    constexpr ColorRGB<T> linearToSRgb(ColorRGB<T> in) noexcept {
        return ColorRGB<T> { linearToSRgb(in.x), linearToSRgb(in.y), linearToSRgb(in.z) };
    }

}
