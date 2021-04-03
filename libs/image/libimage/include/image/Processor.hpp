#pragma once

#include <memory>
#include <vector>

#include <image/Composition.hpp>
#include <image/CoreTypes.hpp>
#include <image/ImageBuf.hpp>
#include <image/NDArray.hpp>
#include <image/Pool.hpp>
#include <image/luts/Lattice3D.hpp>
#include <image/opencl/Program.hpp>

namespace image {

    /**
     * @brief Helper class wrapping a 3D LUT.
     *
     * This is currently required to use the lattice with an OpenCL sampler.
     */
    struct Lut {
        luts::Lattice3D lattice { 32 };
        NDArray<F32> latticeImage;

        void sync() noexcept;
        void reset() noexcept;

        Lut() noexcept;
    };

    template <>
    struct PoolTraits<Lut> {
        static inline Lut construct() noexcept { return Lut {}; }
        static inline void recycle(Lut &lut) noexcept { lut.reset(); }
    };

    /**
     * @brief Represents the application of a LUT to an image with an optional mask.
     *
     * Holds a reference to a LUT object from the pool.
     */
    struct Op {
        PoolLease<Lut> lut;
        std::shared_ptr<AbstractMaskGenerator> maskGen;

        explicit Op(PoolLease<Lut> &&lut) : lut(std::move(lut)) {}
    };

    /**
     * @brief Holds a sequence of operations to apply to an image.
     */
    struct OpSequence {
        std::vector<Op> ops;
    };

    /**
     * @brief Builds a sequence of operations to apply to an image from a number of Layers.
     *
     * The generated OpSequence is only valid for the lifetime of the builder that made it.
     */
    struct OpSequenceBuilder {
        AbstractPool<Lut> &lutPool;
        OpSequence seq;
        Op currentOp;
        bool currentIsNew { true };

        void finaliseOp() noexcept;
        void newOp() noexcept;
        void accumulate(AbstractFilterSpec &filter) noexcept;
        void accumulate(Layer &filter) noexcept;
        void setMask(std::shared_ptr<AbstractMaskGenerator> maskGen) noexcept;

        OpSequence build() noexcept;

        explicit OpSequenceBuilder(AbstractPool<Lut> &lutPool) noexcept;
    };

    /**
     * @brief Maintains runtime data associated with a composition.
     * 
     * This is where ephemeral data associated directly with composition elements (such as loaded images,
     * generated masks, etc.) lives, especially anything which depends on the input image buffer.
     */
    struct CompositionState {
        ImageBuf<F32> input;
        std::map<AbstractMaskGenerator *, Mask> generatedMasks;
        std::unique_ptr<AbstractPool<ImageBuf<F32>>> intermediateImagePool;

        void setInput(const ImageBuf<F32> &image) noexcept;
        Mask &update(AbstractMaskGenerator *maskGen) noexcept;
        Mask &mask(AbstractMaskGenerator *maskGen) noexcept;
    };

    /**
     * @brief Responsible for generating an output image from a Composition.
     *
     * This class maintains a fair amount of state and must be explicitly initialized by calling init().
     */
    struct Processor {
        // TODO: This class can probably be broken-up.

        std::shared_ptr<Composition> composition;

        opencl::Program oclProgram;
        opencl::Kernel oclKernelApplyLut;
        opencl::Kernel oclKernelApplyLutMasked;
        opencl::Kernel oclKernelFinalize;
        opencl::SamplerHandle oclSampler;

        Pool<Lut, 10> lutPool;

        CompositionState state;

        OpSequenceBuilder opSeqBuilder;
        OpSequence opSeq;

        bool areFiltersEnabled { true };

        void init() noexcept;
        void setComposition(std::shared_ptr<Composition> comp) noexcept;
        void update() noexcept;
        void process(ImageBuf<U8> &out) noexcept;

        explicit Processor() noexcept : opSeqBuilder(lutPool) {}
    };

}
