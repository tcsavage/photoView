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
        auto r = rgb.red();
        auto g = rgb.green();
        auto b = rgb.blue();

        auto xMax = std::max(r, std::max(g, b));
        auto xMin = std::min(r, std::min(g, b));
        auto cr = xMax - xMin;
        auto l = (xMax + xMin) / 2.0;
        auto deg60 = 1.0 / 6.0;
        T h = 0.0;
        if (xMax == xMin) {
            h = 0.0;
        } else if (xMax == r) {
            h = deg60 * ((g - b) / cr);
        } else if (xMax == g) {
            h = deg60 * (2.0 + ((b - r) / cr));
        } else if (xMax == b) {
            h = deg60 * (4.0 + ((r - g) / cr));
        }
        T s = 0.0;
        if (xMax != xMin) {
            s = cr / (1.0 - std::abs(xMax + xMin - 1.0));
        }
        return ColorHSL<T>(h, s, l);
    }

    template <std::floating_point T>
    constexpr ColorRGB<T> hslToRgb(const ColorHSL<T> &hsl) noexcept {
        auto h = hsl.hue();
        auto s = hsl.saturation();
        auto l = hsl.luminance();

        auto deg60 = 1.0 / 6.0;

        auto cr = (1.0 - std::abs(2.0 * l - 1.0)) * s;
        auto hp = h / deg60;
        auto xr = cr * (1.0 - std::abs(std::fmod(hp, 2.0) - 1.0));
        auto m = l - cr / 2.0;

        if (hp >= 0 && hp <= 1) {
            return ColorRGB<T>(cr + m, xr + m, m);
        } else if (hp >= 1 && hp <= 2) {
            return ColorRGB<T>(xr + m, cr + m, m);
        } else if (hp >= 2 && hp <= 3) {
            return ColorRGB<T>(m, cr + m, xr + m);
        } else if (hp >= 3 && hp <= 4) {
            return ColorRGB<T>(m, xr + m, cr + m);
        } else if (hp >= 4 && hp <= 5) {
            return ColorRGB<T>(cr + m, m, xr + m);
        } else if (hp >= 5 && hp <= 6) {
            return ColorRGB<T>(xr + m, m, cr + m);
        } else {
            return ColorRGB<T>(m, m, m);
        }
    }

}
