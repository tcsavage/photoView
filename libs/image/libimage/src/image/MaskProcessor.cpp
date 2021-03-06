#include <image/MaskProcessor.hpp>

#include <iostream>

#include <cmrc/cmrc.hpp>

#include <image/opencl/Manager.hpp>

CMRC_DECLARE(image::rc);

namespace image {

    NDArray<U8> MaskProcessor::makeOverlayImageBuf(const Mask &mask) noexcept {
        NDArray<U8> arr { mask.width(), mask.height(), 4 };
        arr.buffer()->setDevice(opencl::Manager::the()->bufferDevice);
        arr.buffer()->deviceMalloc();
        return arr;
    }

    void MaskProcessor::generateOverlayImage(const Mask &mask, NDArray<U8> &out) noexcept {
        // Set args.
        auto saResult = oclKernelGenerateOverlayImage.setArgs(mask.pixelArray, out);
        if (saResult.hasError()) {
            std::cerr << "Error setting kernel args: " << saResult.error().error << " (arg #" << saResult.error().argIdx
                      << ")\n";
            std::terminate();
        }

        // Run the kernel.
        auto runResult = oclKernelGenerateOverlayImage.run(opencl::Manager::the()->queue.getHandle(),
                                                           Shape { mask.width() * mask.height() });
        if (runResult.hasError()) {
            std::cerr << "Error running kernel: " << runResult.error() << "\n";
            std::terminate();
        }

        out.buffer()->copyDeviceToHost();
    }

    MaskProcessor::MaskProcessor() noexcept {
        {
            auto fs = cmrc::image::rc::get_filesystem();
            auto f = fs.open("kernels/maskKernels.cl");
            String src { f.begin(), f.end() };

            auto maybeProg = opencl::Program::fromSource(opencl::Manager::the()->context, src);
            if (maybeProg.hasError()) {
                std::cerr << "Error loading program\n";
                std::terminate();
            }
            oclProgram = std::move(*maybeProg);
        }
        {
            auto buildResult = oclProgram.build();
            if (buildResult.hasError()) {
                std::cerr << "Error building program\n";
                std::terminate();
            }
        }
        {
            auto maybeKern = oclProgram.getKernel("generate_overlay_image_F32_U8");
            if (maybeKern.hasError()) {
                std::cerr << "Error getting kernel from program\n";
                std::terminate();
            }
            oclKernelGenerateOverlayImage = std::move(*maybeKern);
        }
    }

}
