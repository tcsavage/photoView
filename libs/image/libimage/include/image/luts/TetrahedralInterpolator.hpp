#pragma once

#include <optional>

#include <image/CoreTypes.hpp>
#include <image/Color.hpp>
#include <image/NDArray.hpp>
#include <image/luts/Lattice3D.hpp>
#include <image/luts/SimpleCube.hpp>

namespace image::luts {
    
    class TetrahedralInterpolator {
    public:
        using InType = F32;
        using OutType = F32;

        void load(const Lattice3D& lattice) noexcept;

        ColorRGB<OutType> map(const ColorRGB<InType>& color) const noexcept;

    protected:
        const SimpleCube &findCube(ColorRGB<InType> color) const;

    private:
        NDArray<SimpleCube> cubeTable { Shape{} };
        std::size_t latticeSize { 0 };
        mutable ColorRGB<InType> inCache { -1.0f };
        mutable ColorRGB<OutType> outCache { -1.0f };
    };

}
