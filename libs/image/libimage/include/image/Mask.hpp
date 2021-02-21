#pragma once

#include <glm/glm.hpp>

#include <image/CoreTypes.hpp>
#include <image/NDArray.hpp>

namespace image {

    struct Mask {
        NDArray<F32> pixelArray;

        inline F32 *data() noexcept { return reinterpret_cast<F32*>(pixelArray.data()); }
        inline const F32 *data() const noexcept { return reinterpret_cast<const F32*>(pixelArray.data()); }

        inline memory::Size width() const noexcept { return pixelArray.shape().at(0); }
        inline memory::Size height() const noexcept { return pixelArray.shape().at(1); }

        explicit Mask(memory::Size width, memory::Size height) noexcept : pixelArray(Shape{ width, height }) {}
    };

    /**
     * @brief An interface for generating gradients.
     * 
     * Implementations of this interface define masks in an abstract way, and know how to create concrete masks of any size.
     */
    struct AbstractMaskSpec {
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

}
