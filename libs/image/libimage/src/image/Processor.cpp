#include <image/Processor.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>

#include <cmrc/cmrc.hpp>

#include <image/Mask.hpp>
#include <image/opencl/Manager.hpp>

CMRC_DECLARE(image::rc);

namespace image {

    template <class T>
    struct PoolTraits<ImageBuf<T>> {
        static inline ImageBuf<T> construct(memory::Size width, memory::Size height) noexcept {
            auto img = ImageBuf<T> { width, height };
            img.pixelArray.buffer()->device = opencl::Manager::the()->bufferDevice;
            img.pixelArray.buffer()->deviceMalloc();
            return img;
        }
        static inline void recycle(ImageBuf<T> &) noexcept {}
    };

    template <>
    struct PoolTraits<Mask> {
        static inline Mask construct(memory::Size width, memory::Size height) noexcept {
            auto mask = Mask { width, height };
            mask.pixelArray.buffer()->device = opencl::Manager::the()->bufferDevice;
            mask.pixelArray.buffer()->deviceMalloc();
            return mask;
        }
        static inline void recycle(Mask &) noexcept {}
    };

    void Lut::sync() noexcept {
        auto latticeSize = lattice.size;
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

    void Lut::reset() noexcept { lattice.loadIdentity(); }

    Lut::Lut() noexcept {
        lattice.loadIdentity();
        auto latticeSize = lattice.size;
        Shape shape { 4, latticeSize, latticeSize, latticeSize };
        Shape imageShape { latticeSize, latticeSize, latticeSize };
        auto latticeImageDevice = std::make_shared<memory::OpenCLImageDevice>(
            opencl::Manager::the()->context.getHandle(), opencl::Manager::the()->queue.getHandle(), imageShape.dims());
        latticeImage = NDArray<F32>(shape);
        latticeImage.buffer()->device = latticeImageDevice;
        latticeImage.buffer()->deviceMalloc();
    }

    void OpSequenceBuilder::finaliseOp() noexcept { currentOp.lut->sync(); }

    void OpSequenceBuilder::newOp() noexcept {
        // Optimisation: if the current op is new (i.e. effectively a no-op) any mask can be discarded and we can just
        // pretend the existing current op is new again.
        if (currentIsNew) {
            currentOp.mask = nullptr;
            return;
        }
        finaliseOp();
        seq.ops.emplace_back(std::exchange(currentOp, Op { lutPool.acquire() }));
        currentOp.lut->lattice.loadIdentity();
        currentIsNew = true;
    }

    void OpSequenceBuilder::accumulate(AbstractFilterSpec &filter) noexcept {
        currentIsNew = false;
        filter.apply(currentOp.lut->lattice);
    }

    void OpSequenceBuilder::accumulate(Layer &layer) noexcept {
        if (!layer.isEnabled) { return; }
        bool hasActiveMask = layer.mask && layer.mask->isEnabled;
        if (currentOp.mask || hasActiveMask) {
            // We need a new op if:
            // - the current op has one already
            // - or the layer has a mask (regardless of whether the current op has one or not)
            newOp();
        }
        if (hasActiveMask) { setMask(layer.mask->mask()); }
        for (auto &&filter : layer.filters->filterSpecs) {
            if (filter->isEnabled) { accumulate(*filter); }
        }
    }

    void OpSequenceBuilder::setMask(const std::shared_ptr<Mask> &mask) noexcept { currentOp.mask = mask; }

    OpSequence OpSequenceBuilder::build() noexcept {
        if (currentIsNew) {
            currentOp.mask = nullptr;
        } else {
            finaliseOp();
            seq.ops.emplace_back(std::move(currentOp));
            currentOp = Op { lutPool.acquire() };
            currentIsNew = true;
        }
        return std::exchange(seq, OpSequence {});
    }

    OpSequenceBuilder::OpSequenceBuilder(AbstractPool<Lut> &lutPool) noexcept
      : lutPool(lutPool)
      , currentOp(lutPool.acquire()) {}

    void Processor::init() noexcept {
        {
            auto maybeProg = opencl::Manager::the()->programFromResource("kernels/kernels.cl");
            if (maybeProg.hasError()) {
                std::cerr << "Error loading program\n";
                std::terminate();
            }
            oclProgram = std::move(*maybeProg);
        }
        {
            auto maybeKern = oclProgram.getKernel("apply3DLut_F32_F32");
            if (maybeKern.hasError()) {
                std::cerr << "Error getting kernel from program\n";
                std::terminate();
            }
            oclKernelApplyLut = std::move(*maybeKern);
        }
        {
            auto maybeKern = oclProgram.getKernel("apply3DLut_masked_F32_F32");
            if (maybeKern.hasError()) {
                std::cerr << "Error getting kernel from program\n";
                std::terminate();
            }
            oclKernelApplyLutMasked = std::move(*maybeKern);
        }
        {
            auto maybeKern = oclProgram.getKernel("finalize_F32_U8");
            if (maybeKern.hasError()) {
                std::cerr << "Error getting kernel from program\n";
                std::terminate();
            }
            oclKernelFinalize = std::move(*maybeKern);
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

    void Processor::setComposition(std::shared_ptr<Composition> comp) noexcept {
        composition = comp;
        auto &img = *comp->inputImage.data;
        if (!img.pixelArray.buffer()->device) {
            img.pixelArray.buffer()->device = opencl::Manager::the()->bufferDevice;
            img.pixelArray.buffer()->deviceMalloc();
            img.pixelArray.buffer()->copyHostToDevice();
        }
        intermediateImagePool = std::make_unique<Pool<ImageBuf<F32>, 3>>(img.width(), img.height());
    }

    void Processor::update() noexcept {
        if (areFiltersEnabled) {
            for (auto &&layer : composition->layers) {
                opSeqBuilder.accumulate(*layer);
            }
        }
        opSeq = opSeqBuilder.build();
    }

    void Processor::process(ImageBuf<U8> &outFinal) noexcept {
        assert(composition);
        assert(composition->inputImage.data);
        assert(composition->inputImage.data->pixelArray.shape() == outFinal.pixelArray.shape());

        // TODO: This function is rather horrible (mostly due to error handling boilerplate).
        // Would be nice to tidy it up.

        // These optionals maintain ownership of the leased images while they're in use.
        std::optional<PoolLease<ImageBuf<F32>>> intermediateIn;
        std::optional<PoolLease<ImageBuf<F32>>> intermediateOut;

        // The current input image for processing.
        ImageBuf<F32> *currentIn = &(*composition->inputImage.data);

        // Run through the operations.
        for (auto &&op : opSeq.ops) {
            // Acquire a new intermediate image for output.
            intermediateOut = intermediateImagePool->acquire();

            // Convenience.
            auto &latticeImage = op.lut->latticeImage;
            auto &out = **intermediateOut;
            opencl::Kernel *kernel;

            if (op.mask) {
                // Set-up masking kernel to apply LUT.
                kernel = &oclKernelApplyLutMasked;
                auto saResult = kernel->setArgs(
                    latticeImage, oclSampler, currentIn->pixelArray, op.mask->pixelArray, out.pixelArray);
                if (saResult.hasError()) {
                    std::cerr << "Error setting kernel args: " << saResult.error().error << " (arg #"
                              << saResult.error().argIdx << ")\n";
                    std::terminate();
                }
            } else {
                // Set-up non-masking kernel to apply LUT.
                kernel = &oclKernelApplyLut;
                auto saResult = kernel->setArgs(latticeImage, oclSampler, currentIn->pixelArray, out.pixelArray);
                if (saResult.hasError()) {
                    std::cerr << "Error setting kernel args: " << saResult.error().error << " (arg #"
                              << saResult.error().argIdx << ")\n";
                    std::terminate();
                }
            }
            // Run the kernel.
            auto runResult =
                kernel->run(opencl::Manager::the()->queue.getHandle(), Shape { out.width() * out.height() });
            if (runResult.hasError()) {
                std::cerr << "Error running kernel: " << runResult.error() << "\n";
                std::terminate();
            }

            // Update refs.
            intermediateIn = intermediateOut;
            currentIn = &out;
        }

        // Finalize the image and update the output ImageBuf.
        auto saResult = oclKernelFinalize.setArgs(currentIn->pixelArray, outFinal.pixelArray);
        if (saResult.hasError()) {
            std::cerr << "Error setting kernel args: " << saResult.error().error << " (arg #" << saResult.error().argIdx
                      << ")\n";
            std::terminate();
        }
        auto runResult = oclKernelFinalize.run(opencl::Manager::the()->queue.getHandle(),
                                               Shape { outFinal.width() * outFinal.height() });
        if (runResult.hasError()) {
            std::cerr << "Error running kernel: " << runResult.error() << "\n";
            std::terminate();
        }
        outFinal.pixelArray.buffer()->copyDeviceToHost();
    }

}
