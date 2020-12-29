#pragma once

#include <concepts>
#include <iostream>

#include <glm/glm.hpp>

#include <image/CoreTypes.hpp>
#include <image/NDArray.hpp>
#include <image/Color.hpp>
#include <image/luts/Lut.hpp>

namespace image::luts {
    
    template <class Interpolator, class IndexType, class StoredType = Interpolator::OutType>
    requires std::integral<IndexType>
    class FastInterpolator {
    public:
        using IndexColorType = ColorRGB<IndexType>;
        using StoredColorType = ColorRGB<StoredType>;

        FastInterpolator() noexcept
            : size(1 << (sizeof(IndexType) * 8))
            , table(Shape { size, size, size }) {
                std::cerr << "New fast interpolator with size " << size << "\n";
            }

        void load(const Lut& lut) {
            interp.load(lut);
            buildTable();
        }

        StoredColorType map(const IndexColorType &color) const noexcept {
            return table.at(Shape { color.r, color.g, color.b });
        }

        template <class In, class Out>
        ColorRGB<Out> mapConv(const ColorRGB<In> &color) {
            return conv<StoredType, Out>(map(conv<In, IndexType>(color)));
        }

    protected:
        // void buildTable() {
        //     auto sizef = static_cast<F32>(size - 1);
        //     for (std::size_t b = 0; b < size; ++b) {
        //         for (std::size_t g = 0; g < size; ++g) {
        //             for (std::size_t r = 0; r < size; ++r) {
        //                 ColorType in { r, g, b };
        //                 auto inf = static_cast<ColorRGB<F32>>(in) / sizef;
        //                 auto outf = interp.map(inf);
        //                 table.at(Shape { r, g, b }) = static_cast<ColorType>(outf * sizef);
        //             }
        //         }
        //     }
        // }

        void buildTable() {
            for (std::size_t b = 0; b < size; ++b) {
                for (std::size_t g = 0; g < size; ++g) {
                    for (std::size_t r = 0; r < size; ++r) {
                        IndexColorType in { r, g, b };
                        auto out = conv<typename Interpolator::OutType, StoredType>(interp.map(conv<IndexType, typename Interpolator::InType>(in)));
                        table.at(Shape { r, g, b }) = out;
                    }
                }
            }
        }

    private:
        Interpolator interp;
        std::size_t size;
        NDArray<StoredColorType> table;
    };

}
