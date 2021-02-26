#pragma once

#include <image/Mask.hpp>
#include <image/Registry.hpp>

namespace image {

    using MaskGeneratorRegistry = Registry<AbstractMaskGenerator, MaskGeneratorMeta>;

    namespace {
        template <class MaskGenerator>
        void registerMaskGenerator(MaskGeneratorRegistry &reg) noexcept {
            MaskGeneratorMeta meta = MaskGenerator::meta;
            reg.registerType<MaskGenerator>(MaskGenerator::meta.id, std::move(meta));
        }
    }

    inline MaskGeneratorRegistry makeMaskGeneratorRegistry() noexcept {
        MaskGeneratorRegistry reg;

        registerMaskGenerator<LinearGradientMaskSpec>(reg);
        registerMaskGenerator<LumaMaskGenerator>(reg);

        return reg;
    }

}
