#pragma once

#include <image/CoreTypes.hpp>
#include <image/ImageBuf.hpp>
#include <image/Mask.hpp>
#include <image/opencl/Program.hpp>

namespace image {

    class MaskProcessor {
    public:
        ImageBuf<U8, RGBA> makeOverlayImageBuf(const Mask &mask) noexcept;
        void generateOverlayImage(const Mask &mask, ImageBuf<U8, RGBA> &imageBuf) noexcept;

        MaskProcessor() noexcept;

    private:
        opencl::Program oclProgram;
        opencl::Kernel oclKernelGenerateOverlayImage;
    };

}
