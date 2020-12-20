#include <image/luts/LUT.hpp>

namespace image::luts {

    LUT::LUT() noexcept : LUT(0) {}

    LUT::LUT(std::size_t size) noexcept
        : size(size)
        , table(Shape{ size, size, size }) {}

}
