#pragma once

#include <image/Filters.hpp>
#include <image/Registry.hpp>

namespace image {

    using FilterRegistry = Registry<AbstractFilterSpec, FilterMeta>;

    namespace {
        template <class Filter>
        void registerFilter(FilterRegistry &reg) noexcept {
            FilterMeta meta = Filter::meta;
            reg.registerType<Filter>(Filter::meta.id, std::move(meta));
        }
    }

    inline FilterRegistry makeFilterRegistry() noexcept {
        FilterRegistry reg;

        registerFilter<ExposureFilterSpec>(reg);
        registerFilter<LutFilterSpec>(reg);
        registerFilter<SaturationFilterSpec>(reg);
        registerFilter<ContrastFilterSpec>(reg);

        return reg;
    }

}
