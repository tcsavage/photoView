#pragma once

#include <image/Expected.hpp>
#include <image/Filters.hpp>
#include <image/Forward.hpp>

#include "Serialization.hpp"

namespace image::serialization {

    namespace pt = boost::property_tree;

    // Implementations

    struct FilterSerialization {
        virtual void write(const WriteContext &, pt::ptree &, const AbstractFilterSpec *) const noexcept {}
        virtual Expected<void, ReadError>
        read(const ReadContext &, const pt::ptree &, AbstractFilterSpec *) const noexcept {
            return success;
        }
    };

    struct ExposureFilterSerialization final : public FilterSerialization {
        virtual void
        write(const WriteContext &ctx, pt::ptree &tree, const AbstractFilterSpec *filter) const noexcept override;
        virtual Expected<void, ReadError>
        read(const ReadContext &ctx, const pt::ptree &tree, AbstractFilterSpec *filter) const noexcept override;
    };

    struct LutFilterSerialization final : public FilterSerialization {
        virtual void
        write(const WriteContext &ctx, pt::ptree &tree, const AbstractFilterSpec *filter) const noexcept override;
        virtual Expected<void, ReadError>
        read(const ReadContext &ctx, const pt::ptree &tree, AbstractFilterSpec *filter) const noexcept override;
    };

    struct SaturationFilterSerialization final : public FilterSerialization {
        virtual void
        write(const WriteContext &ctx, pt::ptree &tree, const AbstractFilterSpec *filter) const noexcept override;
        virtual Expected<void, ReadError>
        read(const ReadContext &ctx, const pt::ptree &tree, AbstractFilterSpec *filter) const noexcept override;
    };

    struct ContrastFilterSerialization final : public FilterSerialization {
        virtual void
        write(const WriteContext &ctx, pt::ptree &tree, const AbstractFilterSpec *filter) const noexcept override;
        virtual Expected<void, ReadError>
        read(const ReadContext &ctx, const pt::ptree &tree, AbstractFilterSpec *filter) const noexcept override;
    };

    struct ChannelMixerFilterSerialization final : public FilterSerialization {
        virtual void
        write(const WriteContext &ctx, pt::ptree &tree, const AbstractFilterSpec *filter) const noexcept override;
        virtual Expected<void, ReadError>
        read(const ReadContext &ctx, const pt::ptree &tree, AbstractFilterSpec *filter) const noexcept override;
    };

    // Registry

    namespace {
        template <class Filter, class Serialization>
        void registerFilterSerialization(FilterSerializationRegistry &reg) noexcept {
            FilterMeta meta = Filter::meta;
            reg.registerType<Serialization>(Filter::meta.id, std::move(meta));
        }
    }

    inline FilterSerializationRegistry makeFilterSerializationRegistry() noexcept {
        FilterSerializationRegistry reg;

        registerFilterSerialization<ExposureFilterSpec, ExposureFilterSerialization>(reg);
        registerFilterSerialization<LutFilterSpec, LutFilterSerialization>(reg);
        registerFilterSerialization<SaturationFilterSpec, SaturationFilterSerialization>(reg);
        registerFilterSerialization<ContrastFilterSpec, ContrastFilterSerialization>(reg);
        registerFilterSerialization<ChannelMixerFilterSpec, ChannelMixerFilterSerialization>(reg);

        return reg;
    }

}
