#pragma once

#include <atomic>
#include <execution>
#include <memory>
#include <optional>
#include <ranges>
#include <variant>
#include <vector>

#include <image/Color.hpp>
#include <image/CoreTypes.hpp>
#include <image/ImageBuf.hpp>
#include <image/Precalculator.hpp>
#include <image/filters/Concepts.hpp>
#include <image/filters/Filter.hpp>
#include <image/luts/Lut.hpp>

namespace image {

    template <class Out, class In>
    void convImage(const ImageBuf<In> &in, ImageBuf<Out> &out) noexcept {
        std::transform(
            std::execution::par_unseq,
            std::ranges::cbegin(in), std::ranges::cend(in), std::ranges::begin(out),
            [] (const ColorRGB<In> &color) {
                return conv<Out, In>(color);
            }
        );
    }

    template <filters::FilterImpl FI, class In, class Out, bool PrecalculateFilter = true>
    struct ImageProcessor {
        ImageBuf<In> original;
        ImageBuf<Out> viewportOutput;
        filters::Filter<FI, In, PrecalculateFilter> filter;

        std::atomic<bool> isProcessingEnabled { true };

        void setOriginal(const ImageBuf<In> &buf) noexcept {
            original = buf;
            viewportOutput = ImageBuf<Out>(buf.width(), buf.height());
        }

        void setProcessingEnabled(bool processingEnabled) noexcept {
            isProcessingEnabled = processingEnabled;
        }

        void process() noexcept {
            if (isProcessingEnabled) {
                std::transform(
                    std::execution::par_unseq,
                    std::ranges::cbegin(original), std::ranges::cend(original), std::ranges::begin(viewportOutput),
                    [this] (const ColorRGB<In> &color) {
                        return conv<Out>(filter.applyToColor(color));
                    }
                );
            } else {
                std::transform(
                    std::execution::par_unseq,
                    std::ranges::cbegin(original), std::ranges::cend(original), std::ranges::begin(viewportOutput),
                    [this] (const ColorRGB<In> &color) {
                        return conv<Out>(color);
                    }
                );
            }
        }
    };

}
