#pragma once

#include <optional>

#include <image/Color.hpp>
#include <image/CoreTypes.hpp>
#include <image/Mask.hpp>
#include <image/PolyVal.hpp>
#include <image/Resource.hpp>
#include <image/luts/Lattice3D.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>

namespace image {

    struct FilterMeta {
        String id;
        String name;
    };

    struct AbstractFilterSpec {
        bool isEnabled { true };

        virtual const FilterMeta &getMeta() const noexcept = 0;

        virtual bool isSeparable() const noexcept { return false; }
        virtual bool isLinear() const noexcept { return false; }

        virtual void update() noexcept {}

        virtual void apply(luts::Lattice3D &lattice) const noexcept = 0;
    };

    // FilterSpec is a value containing any filter spec implementation.
    using FilterSpec = PolyVal<AbstractFilterSpec>;

    struct ExposureFilterSpec final : public AbstractFilterSpec {
        F32 exposureEvs { 0.0f };
        F32 exposureFactor { 1.0f };  // Derived from exposureEvs

        static inline FilterMeta meta { "filters.exposure", "Exposure" };

        virtual void update() noexcept override;

        virtual const FilterMeta &getMeta() const noexcept override { return meta; }
        virtual bool isSeparable() const noexcept override { return true; }
        virtual bool isLinear() const noexcept override { return true; }

        virtual void apply(luts::Lattice3D &lattice) const noexcept override;
    };

    struct LutFilterSpec final : AbstractFilterSpec {
        LutResource lut;
        F32 strength { 1.0f };

        luts::TetrahedralInterpolator interp;

        static inline FilterMeta meta { "filters.lut", "3D LUT" };

        virtual const FilterMeta &getMeta() const noexcept override { return meta; }
        virtual void update() noexcept override;
        virtual void apply(luts::Lattice3D &lattice) const noexcept override;
    };

}
