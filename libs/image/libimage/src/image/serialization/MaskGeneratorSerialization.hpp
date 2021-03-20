#pragma once

#include <image/Expected.hpp>
#include <image/Forward.hpp>
#include <image/Mask.hpp>

#include "Serialization.hpp"

namespace image::serialization {

    namespace pt = boost::property_tree;

    // Implementations

    struct MaskGeneratorSerialization {
        virtual void write(const WriteContext &, pt::ptree &, const AbstractMaskGenerator *) const noexcept {}
        virtual Expected<void, ReadError>
        read(const ReadContext &, const pt::ptree &, AbstractMaskGenerator *) const noexcept {
            return success;
        }
    };

    struct LumaMaskGeneratorSerialization final : public MaskGeneratorSerialization {
        virtual void
        write(const WriteContext &ctx, pt::ptree &tree, const AbstractMaskGenerator *filter) const noexcept override;
        virtual Expected<void, ReadError>
        read(const ReadContext &ctx, const pt::ptree &tree, AbstractMaskGenerator *filter) const noexcept override;
    };

    struct LinearGradientMaskGeneratorSerialization final : public MaskGeneratorSerialization {
        virtual void
        write(const WriteContext &ctx, pt::ptree &tree, const AbstractMaskGenerator *filter) const noexcept override;
        virtual Expected<void, ReadError>
        read(const ReadContext &ctx, const pt::ptree &tree, AbstractMaskGenerator *filter) const noexcept override;
    };

    // Registry

    namespace {
        template <class MaskGenerator, class Serialization>
        void registerMaskGeneratorSerialization(MaskGeneratorSerializationRegistry &reg) noexcept {
            MaskGeneratorMeta meta = MaskGenerator::meta;
            reg.registerType<Serialization>(MaskGenerator::meta.id, std::move(meta));
        }
    }

    inline MaskGeneratorSerializationRegistry makeMaskGeneratorSerializationRegistry() noexcept {
        MaskGeneratorSerializationRegistry reg;

        registerMaskGeneratorSerialization<LumaMaskGenerator, LumaMaskGeneratorSerialization>(reg);
        registerMaskGeneratorSerialization<LinearGradientMaskSpec, LinearGradientMaskGeneratorSerialization>(reg);

        return reg;
    }

}
