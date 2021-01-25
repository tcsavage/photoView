#include <image/ImageProcessor.hpp>

#include <cmrc/cmrc.hpp>

CMRC_DECLARE(image::rc);

namespace image {

    void ImageProcessorBase::init() {
        {
            auto fs = cmrc::image::rc::get_filesystem();
            auto f = fs.open("kernels/processors.ocl");
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
            auto maybeKern = oclProgram.getKernel("apply3DLut_F32_F32");
            if (maybeKern.hasError()) {
                std::cerr << "Error getting kernel from program\n";
                std::terminate();
            }
            oclKernelApplyLutF32F32 = std::move(*maybeKern);
        }
        {
            auto maybeKern = oclProgram.getKernel("gammaCorrect_F32_U8");
            if (maybeKern.hasError()) {
                std::cerr << "Error getting kernel from program\n";
                std::terminate();
            }
            oclKernelGammaCorrectF32U8 = std::move(*maybeKern);
        }
        {
            cl_int ret;
            cl_sampler samplerHandle = clCreateSampler(
                opencl::Manager::the()->context.getHandle().get(),
                true, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_LINEAR, &ret
            );
            if (ret != CL_SUCCESS) {
                std::cerr << opencl::Error(ret) << "\n";
                std::terminate();
            }
            oclSampler = opencl::SamplerHandle::takeOwnership(samplerHandle);
        }
        filtersLattice.loadIdentity();
        identityLattice.loadIdentity();
        syncLattice();
    }

    void ImageProcessorBase::setProcessingEnabled(bool processingEnabled) noexcept {
        isProcessingEnabled = processingEnabled;
    }

    void ImageProcessorBase::setInput(const ImageBuf<F32> &img) noexcept {
        input = ImageBuf<F32>(img.width(), img.height());
        input.pixelArray.buffer().device = opencl::Manager::the()->bufferDevice;
        input.pixelArray.buffer().deviceMalloc();
        std::copy(img.begin(), img.end(), input.begin());
        input.pixelArray.buffer().copyHostToDevice();

        intermediateBuffer = memory::Buffer(input.pixelArray.buffer().size, input.pixelArray.buffer().alignment);
        intermediateBuffer.device = opencl::Manager::the()->bufferDevice;
        intermediateBuffer.deviceMalloc();
        
        output = ImageBuf<U8>(img.width(), img.height());
        output.pixelArray.buffer().device = opencl::Manager::the()->bufferDevice;
        output.pixelArray.buffer().deviceMalloc();
    }

    void ImageProcessorBase::syncLattice() noexcept {
        auto &lattice = isProcessingEnabled ? filtersLattice : identityLattice; 
        auto latticeSize = lattice.size;
        Shape shape { 4, latticeSize, latticeSize, latticeSize };
        if (latticeImage.shape() != shape) {
            Shape imageShape { latticeSize, latticeSize, latticeSize };
            auto latticeImageDevice = std::make_shared<memory::OpenCLImageDevice>(
                opencl::Manager::the()->context.getHandle(),
                opencl::Manager::the()->queue.getHandle(),
                imageShape
            );
            latticeImage = NDArray<F32>(shape);
            latticeImage.buffer().device = latticeImageDevice;
            latticeImage.buffer().deviceMalloc();
        }
        for (std::size_t b = 0 ; b < latticeSize ; ++b) {
            for (std::size_t g = 0 ; g < latticeSize ; ++g) {
                for (std::size_t r = 0 ; r < latticeSize ; ++r) {
                    const auto &color = lattice.table.at(r, g, b);
                    latticeImage.at(0, r, g, b) = color.r;
                    latticeImage.at(1, r, g, b) = color.g;
                    latticeImage.at(2, r, g, b) = color.b;
                    latticeImage.at(3, r, g, b) = 0;
                }
            }
        }
        latticeImage.buffer().copyHostToDevice();
    }

    [[gnu::noinline, gnu::flatten]]
    void ImageProcessorBase::process() noexcept {
        {
            auto saResult = oclKernelApplyLutF32F32.setArgs(latticeImage, oclSampler, input.pixelArray, intermediateBuffer);
            if (saResult.hasError()) {
                std::cerr << "Error setting kernel args: " << saResult.error().error << " (arg #" << saResult.error().argIdx << ")\n";
                std::terminate();
            }
            auto runResult = oclKernelApplyLutF32F32.run(opencl::Manager::the()->queue.getHandle(), Shape { output.width() * output.height() });
            if (runResult.hasError()) {
                std::cerr << "Error running kernel: " << runResult.error() << "\n";
                std::terminate();
            }
        }
        {
            auto saResult = oclKernelGammaCorrectF32U8.setArgs(intermediateBuffer, output.pixelArray);
            if (saResult.hasError()) {
                std::cerr << "Error setting kernel args: " << saResult.error().error << " (arg #" << saResult.error().argIdx << ")\n";
                std::terminate();
            }
            auto runResult = oclKernelGammaCorrectF32U8.run(opencl::Manager::the()->queue.getHandle(), Shape { output.width() * output.height() });
            if (runResult.hasError()) {
                std::cerr << "Error running kernel: " << runResult.error() << "\n";
                std::terminate();
            }
        }
        output.pixelArray.buffer().copyDeviceToHost();
    }

}
