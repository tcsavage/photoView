#pragma once

#include <istream>
#include <ostream>

#include <image/CoreTypes.hpp>
#include <image/NDArray.hpp>
#include <image/Color.hpp>
#include <image/luts/Lattice3D.hpp>

namespace image::luts {

    struct CubeFile {
        CubeFile() noexcept {}

        CubeFile(Lattice3D &lattice) noexcept;
        
        Lattice3D lattice() const noexcept;
        
        friend std::istream &operator>>(std::istream &input, CubeFile &data);
        friend std::ostream &operator<<(std::ostream &output, const CubeFile &data);

        std::size_t size { 0 };
        String title { "" };
        ColorRGB<F32> domainMin { 0.0f, 0.0f, 0.0f };
        ColorRGB<F32> domainMax { 1.0f, 1.0f, 1.0f };
        NDArray<ColorRGB<F32>> table { Shape{} };
    };

}
