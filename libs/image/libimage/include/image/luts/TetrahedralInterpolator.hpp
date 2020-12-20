#pragma once

#include <optional>

#include <image/CoreTypes.hpp>
#include <image/Color.hpp>
#include <image/NDArray.hpp>
#include <image/luts/LUT.hpp>
#include <image/luts/SimpleCube.hpp>

namespace image::luts {
    
    class TetrahedralInterpolator {
    public:
        void load(const LUT& lut) noexcept;

        ColorRGB map(const ColorRGB& color) const noexcept;

    protected:
        const SimpleCube &findCube(ColorRGB color) const;

    private:
        NDArray<SimpleCube> cubeTable { Shape{} };
        std::size_t lutSize { 0 };
        mutable ColorRGB inCache { -1.0f };
        mutable ColorRGB outCache { -1.0f };
    };

}
