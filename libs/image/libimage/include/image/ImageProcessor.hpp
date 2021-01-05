#pragma once

#include <atomic>
#include <execution>
#include <memory>
#include <optional>
#include <ranges>
#include <tuple>
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
        ImageBuf<In> viewportProcessed;
        ImageBuf<Out> viewportOutput;
        filters::Filter<FI, In, PrecalculateFilter> filter;

        std::atomic<bool> isDirty { true };

        void setOriginal(const ImageBuf<In> &buf) noexcept {
            isDirty = true;
            original = buf;
            viewportProcessed = ImageBuf<In>(buf.width(), buf.height());
            viewportOutput = ImageBuf<Out>(buf.width(), buf.height());
        }

        void process() noexcept {
            if (!isDirty) { return; }
            isDirty = false;
            std::transform(
                std::execution::par_unseq,
                std::ranges::cbegin(original), std::ranges::cend(original), std::ranges::begin(viewportOutput),
                [this] (const ColorRGB<In> &color) {
                    return conv<Out>(filter.applyToColor(color));
                }
            );
        }
    };

}
