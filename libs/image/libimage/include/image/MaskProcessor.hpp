#pragma once

#include <image/CoreTypes.hpp>
#include <image/Mask.hpp>
#include <image/NDArray.hpp>
#include <image/opencl/Program.hpp>

namespace image {

    class MaskProcessor {
    public:
        NDArray<U8> makeOverlayImageBuf(const Mask &mask) noexcept;
        void generateOverlayImage(const Mask &mask, NDArray<U8> &imageBuf) noexcept;

        MaskProcessor() noexcept;

    private:
        opencl::Program oclProgram;
        opencl::Kernel oclKernelGenerateOverlayImage;
    };

}
