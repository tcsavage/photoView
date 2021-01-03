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
    class FastInterpolator : public Interpolator {
    public:
        using InType = IndexType;
        using OutType = StoredType;
        using IndexColorType = ColorRGB<IndexType>;
        using StoredColorType = ColorRGB<StoredType>;

        FastInterpolator() noexcept
            : size(1 << (sizeof(IndexType) * 8))
            , table(Shape { size, size, size }) {
                std::cerr << "New fast interpolator with size " << size << "\n";
            }

        void load(const Lut& lut) {
            Interpolator::load(lut);
            buildTable();
        }

        StoredColorType map(const IndexColorType &color) const noexcept {
            return table.at(Shape { color.r, color.g, color.b });
        }

        template <class In, class Out>
        ColorRGB<Out> mapConv(const ColorRGB<In> &color) {
            return conv<Out, StoredType>(map(conv<IndexType, In>(color)));
        }

        void buildTable() {
            for (std::size_t b = 0; b < size; ++b) {
                for (std::size_t g = 0; g < size; ++g) {
                    for (std::size_t r = 0; r < size; ++r) {
                        IndexColorType in { r, g, b };
                        auto out = conv<StoredType, typename Interpolator::OutType>(Interpolator::map(conv<typename Interpolator::InType, IndexType>(in)));
                        table.at(Shape { r, g, b }) = out;
                    }
                }
            }
        }

    private:
        std::size_t size;
        NDArray<StoredColorType> table;
    };

}
