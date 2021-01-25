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
#include <image/opencl/BufferDevice.hpp>
#include <image/opencl/Manager.hpp>
#include <image/opencl/Program.hpp>
#include <image/filters/Concepts.hpp>
#include <image/filters/Filter.hpp>
#include <image/filters/FilterStack.hpp>
#include <image/luts/Lattice3D.hpp>

namespace image {

    struct ImageProcessorBase {
        ImageBuf<F32> input;
        ImageBuf<U8> output;
        memory::Buffer intermediateBuffer;

        luts::Lattice3D identityLattice { 64 };
        luts::Lattice3D filtersLattice { 64 };
        NDArray<F32> latticeImage;

        std::atomic<bool> isProcessingEnabled { true };

        opencl::Program oclProgram;
        opencl::Kernel oclKernelApplyLutF32F32;
        opencl::Kernel oclKernelGammaCorrectF32U8;
        opencl::SamplerHandle oclSampler;

        void init();
        void setProcessingEnabled(bool processingEnabled) noexcept;
        void setInput(const ImageBuf<F32> &img) noexcept;
        void syncLattice() noexcept;
        void process() noexcept;

        virtual ~ImageProcessorBase() {}
    };

    template <filters::FilterImpl<F32>... Impls>
    struct ImageProcessor final : public ImageProcessorBase {
        filters::FilterStack<F32, false, Impls...> filterStack;

        ImageProcessor() noexcept {
            update();
        }

        void update() noexcept {
            filtersLattice.fromFunction([this](const ColorRGB<F32> &c) {
                return applyToColor(c, filterStack);
            });
            syncLattice();
        }

        template <class Impl>
        requires filters::FilterImpl<Impl, F32>
        constexpr filters::Filter<F32, Impl, false> &getFilter() noexcept {
            return filters::get<Impl, F32, false>(filterStack);
        }

        virtual ~ImageProcessor() {}
    };

}
