#pragma once

#include <glm/glm.hpp>

#include <image/CoreTypes.hpp>
#include <image/NDArray.hpp>
#include <image/Color.hpp>
#include <image/luts/LUT.hpp>

namespace image::luts {
    
    template <class T, class I>
    class FastInterpolator {
    public:
        using ColorType = glm::vec<3, T, glm::packed_highp>;

        FastInterpolator() noexcept
            : size(1 << (sizeof(T) * 8))
            , table(Shape { size, size, size }) {}

        void load(const LUT& lut) {
            interp.load(lut);
            buildTable();
        }

        ColorRGB map(const ColorType& color) const noexcept {
            return table.at(Shape { color.r, color.g, color.b });
        }

    protected:
        void buildTable() {
            auto sizef = static_cast<F32>(size - 1);
            for (std::size_t b = 0; b < size; ++b) {
                for (std::size_t g = 0; g < size; ++g) {
                    for (std::size_t r = 0; r < size; ++r) {
                        ColorType in { r, g, b };
                        auto inf = static_cast<ColorRGB>(in) / sizef;
                        auto outf = interp.map(inf);
                        table.at(Shape { r, g, b }) = static_cast<ColorType>(outf * sizef);
                    }
                }
            }
        }

    private:
        I interp;
        std::size_t size;
        NDArray<ColorType> table;
    };

}
