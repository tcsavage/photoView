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
        constexpr ColorRGB(Args&& ... args) noexcept : glm::vec<3, T, glm::defaultp>(std::forward<Args>(args)...) {}
    };

    template <class In, class Out>
    requires (std::unsigned_integral<In> && std::unsigned_integral<Out>) || (std::floating_point<In> && std::floating_point<Out>)
    Out conv(const In &in) {
        return in;
    }

    template <std::unsigned_integral In, std::floating_point Out>
    Out conv(const In &in) {
        return static_cast<Out>(in) / static_cast<Out>(std::numeric_limits<In>::max());
    }

    template <std::floating_point In, std::unsigned_integral Out>
    Out conv(const In &in) {
        return static_cast<Out>(in * static_cast<In>(std::numeric_limits<Out>::max()));
    }

    template <class In, class Out>
    ColorRGB<Out> conv(const ColorRGB<In> &in) {
        return ColorRGB<Out> { conv<In, Out>(in.r), conv<In, Out>(in.g), conv<In, Out>(in.b) };
    }

    template <std::floating_point T>
    T mix(T factor, T a, T b) {
        return ((1 - factor) * a) + (factor * b);
    }

    template <std::unsigned_integral T, std::floating_point F = F32>
    T mix(F factor, T a, T b) {
        return conv<F, T>(mix(factor, conv<T, F>(a), conv<T, F>(b)));
    }

    template <class T, std::floating_point F = F32>
    ColorRGB<T> mix(F factor, ColorRGB<T> a, ColorRGB<T> b) {
        return ColorRGB<T> {
            mix<T, F>(factor, a.r, b.r),
            mix<T, F>(factor, a.g, b.g),
            mix<T, F>(factor, a.b, b.b)
        };
    }

}
