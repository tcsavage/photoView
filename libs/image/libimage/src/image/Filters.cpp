#include <image/Filters.hpp>

namespace image {

    template <class T>
    constexpr T evToScale(T evs) {
        return std::exp2(evs);
    }

    void ExposureFilterSpec::update() noexcept { exposureFactor = evToScale(exposureEvs); }

    void ExposureFilterSpec::apply(luts::Lattice3D &lattice) const noexcept {
        lattice.accumulate([this](ColorRGB<F32> &c) {
            ColorRGB<F32> linear = sRgbToLinear(c) * exposureFactor;
            return linearToSRgb(linear);
        });
    }

    void LutFilterSpec::update() noexcept {
        if (!lut.data) { lut.load(); }
        interp.load(*lut.data);
    }

    void LutFilterSpec::apply(luts::Lattice3D &lattice) const noexcept {
        if (lut.data) {
            lattice.accumulate([this](ColorRGB<F32> &c) { return mix(strength, interp.map(c), c); });
        }
    }

    void SaturationFilterSpec::apply(luts::Lattice3D &lattice) const noexcept {
        lattice.accumulate([this](ColorRGB<F32> &c) {
            auto hsl = rgbToHsl(c);
            hsl.saturation() *= multiplier;
            return hslToRgb(hsl);
        });
    }

}
