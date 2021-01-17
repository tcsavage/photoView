#include <image/luts/Lattice3D.hpp>

namespace image::luts {

    Lattice3D::Lattice3D() noexcept : Lattice3D(0) {}

    Lattice3D::Lattice3D(std::size_t size) noexcept
        : size(size)
        , table(Shape{ size, size, size }) {}

}
