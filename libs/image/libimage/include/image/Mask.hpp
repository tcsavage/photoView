#pragma once

#include <glm/glm.hpp>

#include <image/CoreTypes.hpp>
#include <image/ImageBuf.hpp>
#include <image/NDArray.hpp>

namespace image {

    struct Mask {
        NDArray<F32> pixelArray;

        inline F32 *data() noexcept { return reinterpret_cast<F32 *>(pixelArray.data()); }
        inline const F32 *data() const noexcept { return reinterpret_cast<const F32 *>(pixelArray.data()); }

        inline memory::Size width() const noexcept { return pixelArray.shape().at(0); }
        inline memory::Size height() const noexcept { return pixelArray.shape().at(1); }

        explicit Mask(memory::Size width, memory::Size height) noexcept : pixelArray(Shape { width, height }) {}
    };

    struct AbstractMaskGenerator {
        virtual void generate(const ImageBuf<F32> &img, Mask &mask) const noexcept = 0;

        virtual ~AbstractMaskGenerator() noexcept {}
    };

    struct LumaMaskGenerator : public AbstractMaskGenerator {
        virtual void generate(const ImageBuf<F32> &img, Mask &mask) const noexcept override;

        virtual ~LumaMaskGenerator() noexcept {}
    };

    /**
     * @brief An interface for generating gradient masks.
     *
     * Implementations of this interface define masks in an abstract way, and know how to create concrete masks of any
     * size.
     */
    struct AbstractMaskSpec : public AbstractMaskGenerator {
        virtual void generate(const ImageBuf<F32> &, Mask &mask) const noexcept override { generate(mask); }
        virtual void generate(Mask &mask) const noexcept = 0;

        virtual ~AbstractMaskSpec() noexcept {}
    };

    /**
     * @brief Defines a linear gradient between two 2D points.
     */
    struct LinearGradientMaskSpec : public AbstractMaskSpec {
        glm::vec2 from;
        glm::vec2 to;

        virtual void generate(Mask &mask) const noexcept override;

        constexpr LinearGradientMaskSpec(const glm::vec2 &from, const glm::vec2 &to) noexcept : from(from), to(to) {}

        virtual ~LinearGradientMaskSpec() noexcept {}
    };

    class GeneratedMask {
    public:
        inline std::shared_ptr<Mask> mask() const noexcept { return mask_; }

        inline void setGenerator(std::shared_ptr<AbstractMaskGenerator> gen) noexcept { gen_ = gen; }

        void update(const ImageBuf<F32> &img) const noexcept;

        GeneratedMask(std::shared_ptr<AbstractMaskGenerator> gen) noexcept : gen_(gen) {}

    protected:
        void ensureMask(memory::Size width, memory::Size height) const noexcept;

    private:
        mutable std::shared_ptr<Mask> mask_;
        std::shared_ptr<AbstractMaskGenerator> gen_;
    };

}
