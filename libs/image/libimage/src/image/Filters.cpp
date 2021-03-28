#include <image/Filters.hpp>

#include <glm/geometric.hpp>

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
            lattice.accumulate([this](ColorRGB<F32> &c) { return mix(strength, c, interp.map(c)); });
        }
    }

    void SaturationFilterSpec::apply(luts::Lattice3D &lattice) const noexcept {
        auto mat = saturationMatrix(multiplier);
        lattice.accumulate([this, &mat](ColorRGB<F32> &c) { return linearToSRgb(mat * sRgbToLinear(c)); });
    }

    void ContrastFilterSpec::apply(luts::Lattice3D &lattice) const noexcept {
        ColorRGB<F32> grey { 0.5 };
        lattice.accumulate(
            [this, &grey](ColorRGB<F32> &c) { return linearToSRgb(mix(factor, grey, sRgbToLinear(c))); });
    }

    void ChannelMixerFilterSpec::apply(luts::Lattice3D &lattice) const noexcept {
        MatrixRGB<F32> mat = matrix;
        F32 rwgt = 0.3086;
        F32 gwgt = 0.6094;
        F32 bwgt = 0.0820;
        ColorRGB<F32> scale { rwgt, gwgt, bwgt };
        if (!preserveLuminosity) {
            lattice.accumulate([this, &mat](ColorRGB<F32> &c) { return linearToSRgb(mat * sRgbToLinear(c)); });
        } else {
            lattice.accumulate([this, &mat, &scale](ColorRGB<F32> &c) {
                ColorRGB<F32> c1 = sRgbToLinear(c);
                ColorRGB<F32> c2 = mat * c1;
                F32 l1 = glm::dot(glm::vec3(scale), glm::vec3(c1));
                F32 l2 = glm::dot(glm::vec3(scale), glm::vec3(c2));
                return linearToSRgb(ColorRGB<F32> { c2 * (l1 / l2) });
            });
        }
    }

}
