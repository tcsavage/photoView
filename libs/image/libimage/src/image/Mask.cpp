#include <image/Mask.hpp>

#include <cassert>

#include <image/IO.hpp>
#include <image/Stopwatch.hpp>
#include <image/opencl/Manager.hpp>

namespace image {

    struct LinearGradientProjection {
        glm::vec2 from;
        glm::vec2 to;

        F32 a;
        F32 b;

        F32 c1;
        F32 c2;

        constexpr F32 project(const glm::vec2 &point) const noexcept { return a * point.x + b * point.y; }

        constexpr F32 map(const glm::vec2 &point) const noexcept {
            auto proj = project(point);
            if (proj <= c1) { return 0.0f; }
            if (proj >= c2) { return 1.0f; }
            return (proj - c1) / (c2 - c1);
        }

        constexpr LinearGradientProjection(glm::vec2 from, glm::vec2 to)
          : from(from)
          , to(to)
          , a(to.x - from.x)
          , b(to.y - from.y)
          , c1(project(from))
          , c2(project(to)) {
            assert(c2 >= c1);
        }
    };

    void LinearGradientMaskSpec::generate(Mask &mask) const noexcept {
        // STOPWATCH("Generating linear gradient mask");
        LinearGradientProjection proj { from, to };
        glm::vec2 size { static_cast<F32>(mask.width()), static_cast<F32>(mask.height()) };
        #pragma omp parallel for
        for (memory::Size y = 0; y < mask.height(); ++y) {
            #pragma omp parallel for
            for (memory::Size x = 0; x < mask.width(); ++x) {
                glm::vec2 pos = glm::vec2 { static_cast<F32>(x), static_cast<F32>(y) } / size;
                auto value = proj.map(pos);
                mask.pixelArray.at(x, y) = value;
            }
        }
    }

    void LumaMaskGenerator::generate(const ImageBuf<F32> &img, Mask &mask) const noexcept {
        STOPWATCH("Generating luma mask");
        glm::vec2 size { static_cast<F32>(mask.width()), static_cast<F32>(mask.height()) };
        #pragma omp parallel for
        for (memory::Size y = 0; y < mask.height(); ++y) {
            #pragma omp parallel for
            for (memory::Size x = 0; x < mask.width(); ++x) {
                const auto &color = img.at(x, y);
                auto value = (color.r + color.g + color.b) / 3.0f;
                mask.pixelArray.at(x, y) = value;
            }
        }
    }

    void GeneratedMask::update(const ImageBuf<F32> &img) const noexcept {
        ensureMask(img.width(), img.height());
        gen_->generate(img, *mask());
        mask_->pixelArray.buffer()->copyHostToDevice();
    }

    void GeneratedMask::ensureMask(memory::Size width, memory::Size height) const noexcept {
        if (!mask_ || mask_->width() != width || mask_->height() != height) {
            mask_ = std::make_shared<Mask>(width, height);
            mask_->pixelArray.buffer()->device = opencl::Manager::the()->bufferDevice;
            mask_->pixelArray.buffer()->deviceMalloc();
        }
    }

}
