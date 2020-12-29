#pragma once

#include <optional>

#include <image/CoreTypes.hpp>
#include <image/Color.hpp>
#include <image/NDArray.hpp>
#include <image/luts/Lut.hpp>
#include <image/luts/SimpleCube.hpp>

namespace image::luts {
    
    class TetrahedralInterpolator {
    public:
        using InType = F32;
        using OutType = F32;
        void load(const Lut& lut) noexcept;

        ColorRGB<F32> map(const ColorRGB<F32>& color) const noexcept;

    protected:
        const SimpleCube &findCube(ColorRGB<F32> color) const;

    private:
        NDArray<SimpleCube> cubeTable { Shape{} };
        std::size_t lutSize { 0 };
        mutable ColorRGB<F32> inCache { -1.0f };
        mutable ColorRGB<F32> outCache { -1.0f };
    };

}
