#include <image/Look.hpp>

#include <cassert>
#include <iostream>

#include <cmrc/cmrc.hpp>

#include <image/opencl/Manager.hpp>

CMRC_DECLARE(image::rc);

namespace image {

    FilterSpec &Look::addFilter(FilterSpec &&filter) noexcept { return filterSpecs.emplace_back(std::move(filter)); }

    void Processor::init() noexcept {
        {
            auto fs = cmrc::image::rc::get_filesystem();
            auto f = fs.open("kernels/apply3DLut_F32_U8.cl");
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
            auto maybeKern = oclProgram.getKernel("apply3DLut_F32_U8");
            if (maybeKern.hasError()) {
                std::cerr << "Error getting kernel from program\n";
                std::terminate();
            }
            oclKernel = std::move(*maybeKern);
        }
        {
            cl_int ret;
            cl_sampler samplerHandle = clCreateSampler(opencl::Manager::the()->context.getHandle().get(),
                                                       true,
                                                       CL_ADDRESS_CLAMP_TO_EDGE,
                                                       CL_FILTER_LINEAR,
                                                       &ret);
            if (ret != CL_SUCCESS) {
                std::cerr << opencl::Error(ret) << "\n";
                std::terminate();
            }
            oclSampler = opencl::SamplerHandle::takeOwnership(samplerHandle);
        }
    }

    void Processor::update() noexcept {
        lattice.loadIdentity();
        if (look) {
            for (auto &fs : look->filterSpecs) {
                fs->apply(lattice);
            }
        }
        syncLattice();
    }

    void Processor::syncLattice() noexcept {
        auto latticeSize = lattice.size;
        Shape shape { 4, latticeSize, latticeSize, latticeSize };
        if (latticeImage.shape() != shape) {
            Shape imageShape { latticeSize, latticeSize, latticeSize };
            auto latticeImageDevice =
                std::make_shared<memory::OpenCLImageDevice>(opencl::Manager::the()->context.getHandle(),
                                                            opencl::Manager::the()->queue.getHandle(),
                                                            imageShape.dims());
            latticeImage = NDArray<F32>(shape);
            latticeImage.buffer()->device = latticeImageDevice;
            latticeImage.buffer()->deviceMalloc();
        }
        for (std::size_t b = 0; b < latticeSize; ++b) {
            for (std::size_t g = 0; g < latticeSize; ++g) {
                for (std::size_t r = 0; r < latticeSize; ++r) {
                    const auto &color = lattice.table.at(r, g, b);
                    latticeImage.at(0, r, g, b) = color.r;
                    latticeImage.at(1, r, g, b) = color.g;
                    latticeImage.at(2, r, g, b) = color.b;
                    latticeImage.at(3, r, g, b) = 0;
                }
            }
        }
        latticeImage.buffer()->copyHostToDevice();
    }

    void Processor::process(ImageBuf<F32> &in, ImageBuf<U8> &out) noexcept {
        assert(in.pixelArray.shape() == out.pixelArray.shape());
        auto saResult = oclKernel.setArgs(latticeImage, oclSampler, in.pixelArray, out.pixelArray);
        if (saResult.hasError()) {
            std::cerr << "Error setting kernel args: " << saResult.error().error << " (arg #" << saResult.error().argIdx
                      << ")\n";
            std::terminate();
        }
        auto runResult = oclKernel.run(opencl::Manager::the()->queue.getHandle(), Shape { out.width() * out.height() });
        if (runResult.hasError()) {
            std::cerr << "Error running kernel: " << runResult.error() << "\n";
            std::terminate();
        }
        out.pixelArray.buffer()->copyDeviceToHost();
    }

}
