#pragma once

#include <glm/glm.hpp>

#include <image/CoreTypes.hpp>
#include <image/ImageBuf.hpp>
#include <image/NDArray.hpp>

namespace image {

    using Mask = ImageBuf<F32, Greyscale>;

    struct MaskGeneratorMeta {
        String id;
        String name;
    };

    struct AbstractMaskGenerator {
        bool isEnabled { true };
        
        virtual const MaskGeneratorMeta &getMeta() const noexcept = 0;
        virtual void generate(const ImageBuf<F32> &img, Mask &mask) const noexcept = 0;

        virtual ~AbstractMaskGenerator() noexcept {}
    };

    struct LumaMaskGenerator : public AbstractMaskGenerator {
        static inline MaskGeneratorMeta meta { "maskGenerators.luma", "Luma Mask" };

        virtual const MaskGeneratorMeta &getMeta() const noexcept override { return meta; }

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
        glm::vec2 from { 0.5, 0.3 };
        glm::vec2 to { 0.5, 0.7 };

        static inline MaskGeneratorMeta meta { "maskGenerators.linearGradient", "Linear Gradient Mask" };

        virtual const MaskGeneratorMeta &getMeta() const noexcept override { return meta; }

        virtual void generate(Mask &mask) const noexcept override;

        constexpr LinearGradientMaskSpec() noexcept {}
        constexpr LinearGradientMaskSpec(const glm::vec2 &from, const glm::vec2 &to) noexcept : from(from), to(to) {}

        virtual ~LinearGradientMaskSpec() noexcept {}
    };

}
