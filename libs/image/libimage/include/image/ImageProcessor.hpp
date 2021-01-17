#pragma once

#include <iostream>

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
#include <image/filters/FilterStack.hpp>
#include <image/luts/Lattice3D.hpp>

namespace image {

    template <class In, class Out, bool PrecalculateFilter, filters::FilterImpl<In>... Impls>
    struct ImageProcessor {
        ImageBuf<In> original;
        ImageBuf<Out> viewportOutput;
        filters::FilterStack<In, PrecalculateFilter, Impls...> filterStack;
        Precalculator<Out, Out> precalc;

        std::atomic<bool> isProcessingEnabled { true };

        ImageProcessor() noexcept {
            update();
        }

        void update() noexcept {
            if constexpr(PrecalculateFilter) {
                precalc.buildTable([this](const ColorRGB<Out> &color) {
                    auto fcolor = conv<In>(color);
                    return conv<Out>(applyToColor(fcolor, filterStack));
                });
            }
        }

        void setOriginal(const ImageBuf<In> &buf) noexcept {
            original = buf;
            viewportOutput = ImageBuf<Out>(buf.width(), buf.height());
        }

        void setProcessingEnabled(bool processingEnabled) noexcept {
            isProcessingEnabled = processingEnabled;
        }

        [[gnu::noinline]]
        void process() noexcept {
            if (isProcessingEnabled) {
                if constexpr(PrecalculateFilter) {
                    std::transform(
                        std::execution::par_unseq,
                        std::ranges::cbegin(original), std::ranges::cend(original), std::ranges::begin(viewportOutput),
                        [this] (const ColorRGB<In> &color) {
                            return precalc.map(conv<Out>(color));
                        }
                    );
                } else {
                    std::transform(
                        std::execution::par_unseq,
                        std::ranges::cbegin(original), std::ranges::cend(original), std::ranges::begin(viewportOutput),
                        [this] (const ColorRGB<In> &color) {
                            auto fcolor = conv<In>(color);
                            return conv<Out>(applyToColor(fcolor, filterStack));
                        }
                    );
                }
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

        template <class Impl, class T, bool Preprocess>
        requires filters::FilterImpl<Impl, T>
        constexpr filters::Filter<T, Impl, Preprocess> &getFilter() noexcept {
            return filters::get<Impl>(filterStack);
        }
    };

}
