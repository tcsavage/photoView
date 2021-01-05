#pragma once

#include <concepts>
#include <iostream>

#include <image/CoreTypes.hpp>
#include <image/NDArray.hpp>
#include <image/Color.hpp>

namespace image {
    
    template <std::unsigned_integral IndexType, class StoredType>
    class Precalculator {
    public:
        using IndexColorType = ColorRGB<IndexType>;
        using StoredColorType = ColorRGB<StoredType>;

        Precalculator() noexcept : table(Shape { size, size, size }) {}

        template <class F>
        Precalculator(F f) : Precalculator() { buildTable(std::forward<F>(f)); }

        StoredColorType map(const IndexColorType &color) const noexcept {
            return table.at(color.r, color.g, color.b);
        }

        template <class F>
        void buildTable(F f) {
            for (std::size_t b = 0; b < size; ++b) {
                for (std::size_t g = 0; g < size; ++g) {
                    for (std::size_t r = 0; r < size; ++r) {
                        IndexColorType in { r, g, b };
                        table.at(r, g, b) = f(in);
                    }
                }
            }
        }

    private:
        static constexpr std::size_t size = 1 << (sizeof(IndexType) * 8);
        NDArray<StoredColorType> table;
    };

}
