#pragma once

#include <cstddef>

#include <image/CoreTypes.hpp>
#include <image/NDArray.hpp>
#include <image/Color.hpp>

namespace image::luts {
    
    struct Lattice3D {
        Lattice3D() noexcept;
        Lattice3D(std::size_t size) noexcept;
    
        std::size_t size { 0 };
        ColorRGB<F32> domainMin { 0.0f, 0.0f, 0.0f };
        ColorRGB<F32> domainMax { 1.0f, 1.0f, 1.0f };
        NDArray<ColorRGB<F32>> table { Shape{} };

        template <class F>
        void accumulate(F f) noexcept {
            for (std::size_t b = 0 ; b < size ; ++b) {
                for (std::size_t g = 0 ; g < size ; ++g) {
                    for (std::size_t r = 0 ; r < size ; ++r) {
                        auto &node = table.at(r, g, b);
                        ColorRGB<F32> cOut = f(node);
                        node = cOut;
                    }
                }
            }
        }

        template <class F>
        void fromFunction(F f) noexcept {
            auto maxf = static_cast<F32>(size - 1);
            auto step = 1.0f / maxf;
            for (std::size_t b = 0 ; b < size ; ++b) {
                F32 bf = b * step;
                for (std::size_t g = 0 ; g < size ; ++g) {
                    F32 gf = g * step;
                    for (std::size_t r = 0 ; r < size ; ++r) {
                        F32 rf = r * step;
                        ColorRGB<F32> cIn { rf, gf, bf };
                        ColorRGB<F32> cOut = f(cIn);
                        table.at(r, g, b) = cOut;
                    }
                }
            }
        }

        inline void loadIdentity() noexcept {
            fromFunction([](const ColorRGB<F32> &color) { return color; });
        }
    };

}
