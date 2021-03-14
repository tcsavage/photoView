#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <iostream>

#include <image/Color.hpp>
#include <image/CoreTypes.hpp>
#include <image/ImageBuf.hpp>
#include <image/NDArray.hpp>

namespace image {

    template <class I>
    struct Histogram {
        constexpr static const memory::Size NUM_BUCKETS { std::numeric_limits<I>::max() + 1 };

        std::array<F32, std::numeric_limits<I>::max() + 1> red;
        std::array<F32, std::numeric_limits<I>::max() + 1> green;
        std::array<F32, std::numeric_limits<I>::max() + 1> blue;

        constexpr memory::Size numBuckets() const noexcept { return NUM_BUCKETS; }

        template <class T>
        void generate(const ImageBuf<T, RGB> &img) noexcept {
            std::array<U32, NUM_BUCKETS> ired, igreen, iblue;
            ired.fill(0);
            igreen.fill(0);
            iblue.fill(0);

            // Count values.
            const ColorRGB<T> *pixels = reinterpret_cast<const ColorRGB<T> *>(img.pixelArray.data());
            auto numPixels = img.width() * img.height();
            #pragma omp parallel for
            for (memory::Size i = 0; i < numPixels; ++i) {
                auto ipixel = conv<I, T>(pixels[i]);
                ired[ipixel.x]++;
                igreen[ipixel.y]++;
                iblue[ipixel.z]++;
            }

            // Write to floating point output data.
            // We ignore top and bottom buckets for scaling purposes because it can throw things off when things start to clip.
            auto maxRed = std::max_element(ired.begin() + 1, ired.end() - 1);
            auto maxGreen = std::max_element(igreen.begin() + 1, igreen.end() - 1);
            auto maxBlue = std::max_element(iblue.begin() + 1, iblue.end() - 1);
            auto maxAll = static_cast<F32>(std::max(*maxRed, std::max(*maxGreen, *maxBlue)));
            for (memory::Size i = 0; i < NUM_BUCKETS; i++) {
                red[i] = static_cast<F32>(ired[i]) / maxAll;
                green[i] = static_cast<F32>(igreen[i]) / maxAll;
                blue[i] = static_cast<F32>(iblue[i]) / maxAll;
                std::cerr << red[i] << ", " << green[i] << ", " << blue[i] << "\n";
            }
        }

        void generateTest() noexcept {
            #pragma omp parallel for
            for (memory::Size i = 0; i < NUM_BUCKETS; i++) {
                float x =  static_cast<F32>(i) / static_cast<F32>(NUM_BUCKETS);
                float r = pow(0.5, pow(5 * (x - 0.5), 2));
                float g = pow(0.5, pow(5 * x, 2));
                float b = pow(0.5, pow(5 * (x - 1), 2));
                red[i] = r;
                green[i] = g;
                blue[i] = b;
            }
        }
    };

}
