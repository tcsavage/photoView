#include <image/luts/Lut.hpp>

namespace image::luts {

    Lut::Lut() noexcept : Lut(0) {}

    Lut::Lut(std::size_t size) noexcept
        : size(size)
        , table(Shape{ size, size, size }) {}

}
