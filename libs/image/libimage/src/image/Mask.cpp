#include <image/Mask.hpp>

namespace image {

    Mask makeTestMask(memory::Size width, memory::Size height) noexcept {
        Mask mask { width, height };

        // TODO: this can be improved by creating one row and then copying it.
        auto third = width / 3;
        for (memory::Size y = 0; y < height; ++y) {
            for (memory::Size x = 0; x < width; ++x) {
                if (x < third) {
                    mask.pixelArray.at(x, y) = 0.0f;
                } else if (x < third * 2) {
                    mask.pixelArray.at(x, y) = static_cast<F32>(x - third) / static_cast<F32>(third);
                } else {
                    mask.pixelArray.at(x, y) = 1.0f;
                }
            }
        }

        return mask;
    }

}
