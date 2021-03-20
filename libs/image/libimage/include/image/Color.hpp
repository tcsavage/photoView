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
    using BaseColor3 = glm::vec<3, T, glm::defaultp>;

    template <class T>
    struct ColorRGB final : public BaseColor3<T> {
        constexpr ColorRGB() noexcept : BaseColor3<T>(0) {}

        template <class... Args>
        constexpr ColorRGB(Args &&... args) noexcept : BaseColor3<T>(std::forward<Args>(args)...) {}

        constexpr T &red() noexcept { return BaseColor3<T>::r; }
        constexpr const T &red() const noexcept { return BaseColor3<T>::r; }
        constexpr T &green() noexcept { return BaseColor3<T>::g; }
        constexpr const T &green() const noexcept { return BaseColor3<T>::g; }
        constexpr T &blue() noexcept { return BaseColor3<T>::b; }
        constexpr const T &blue() const noexcept { return BaseColor3<T>::b; }
    };

    template <class T>
    struct ColorHSL final : public BaseColor3<T> {
        constexpr ColorHSL() noexcept : BaseColor3<T>(0) {}

        template <class... Args>
        constexpr ColorHSL(Args &&... args) noexcept : BaseColor3<T>(std::forward<Args>(args)...) {}

        constexpr T &hue() noexcept { return BaseColor3<T>::x; }
        constexpr const T &hue() const noexcept { return BaseColor3<T>::x; }
        constexpr T &saturation() noexcept { return BaseColor3<T>::y; }
        constexpr const T &saturation() const noexcept { return BaseColor3<T>::y; }
        constexpr T &luminance() noexcept { return BaseColor3<T>::z; }
        constexpr const T &luminance() const noexcept { return BaseColor3<T>::z; }
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

    template <class Out, class In>
    constexpr ColorHSL<Out> conv(const ColorHSL<In> &in) noexcept {
        return ColorHSL<Out> { conv<Out, In>(in.r), conv<Out, In>(in.g), conv<Out, In>(in.b) };
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

    template <std::floating_point T>
    constexpr ColorHSL<T> rgbToHsl(const ColorRGB<T> &rgb) noexcept {
        T r = rgb.red();
        T g = rgb.green();
        T b = rgb.blue();

        T v = std::max<T>(std::max<T>(r, g), b);
        T c = v - std::min<T>(std::min<T>(r, g), b);
        T f = 1.0 - std::abs(v + v - c - 1.0);

        T h = 0.0;
        if (c) {
            if (v == r) {
                h = (g - b) / c;
            } else if (v == g) {
                h = 2.0 + (b - r) / c;
            } else {
                h = 4.0 + (r - g) / c;
            }
            if (h < 0.0) { h += 6.0; }
            h /= 6.0;
        }

        T s = 0.0;
        if (f > 0.0) { s = c / f; }

        T l = (v + v - c) / 2.0;

        return ColorHSL<T>(h, s, l);
    }

    template <std::floating_point T>
    constexpr ColorRGB<T> hslToRgb(const ColorHSL<T> &hsl) noexcept {
        auto h = std::clamp<T>(hsl.hue(), 0.0, 1.0) * 360.0;
        auto s = std::clamp<T>(hsl.saturation(), 0.0, 1.0);
        auto l = std::clamp<T>(hsl.luminance(), 0.0, 1.0);

        auto a = s * std::min<T>(l, 1 - l);

        auto kr = std::fmod(h / 30.0, 12.0);
        auto r = l - a * std::max<T>(std::min<T>(kr - 3.0, 9.0 - kr), -1.0);

        auto kg = std::fmod(8.0 + h / 30.0, 12.0);
        auto g = l - a * std::max<T>(std::min<T>(kg - 3.0, 9.0 - kg), -1.0);

        auto kb = std::fmod(4.0 + h / 30.0, 12.0);
        auto b = l - a * std::max<T>(std::min<T>(kb - 3.0, 9.0 - kb), -1.0);

        return ColorRGB<T>(r, g, b);
    }
    }

}
