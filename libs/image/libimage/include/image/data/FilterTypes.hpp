#pragma once

#include <image/Expected.hpp>
#include <image/Filters.hpp>
#include <image/data/Declare.hpp>
#include <image/data/PrimitiveTypes.hpp>

namespace image {

    struct DynFilterRegistry {
        DynFilterRegistry() = delete;

        template <class T>
        static bool registerFilter() {
            TypeInfo<T> &ti = TypeInfoTraits<T>::desc;
            typeInfos.emplace(T::meta.id, &ti);
            return true;
        }

        static AbstractTypeInfo *typeInfo(const String &filterId) noexcept {
            if (auto it = typeInfos.find(filterId); it != typeInfos.end()) {
                return it->second;
            }
            return nullptr;
        }

        inline static std::map<String, AbstractTypeInfo *> typeInfos;
    };

    inline DynamicRef dynFilterRef(AbstractFilterSpec *ptr) {
        return DynamicRef::makeUnchecked(ptr, DynFilterRegistry::typeInfo(ptr->getMeta().id));
    }

}

#define REGISTER_FILTER(type)                                                        \
    struct Filter_##type##_registration {                                            \
        inline static bool isRegistered = DynFilterRegistry::registerFilter<type>(); \
    }

namespace image {

    TYPE(ExposureFilterSpec, { name = "Exposure filter"; });
    REGISTER_FILTER(ExposureFilterSpec);
    PROPERTY(ExposureFilterSpec, exposureEvs, { exposureEvs.name = "Exposure EVs"; });


    TYPE(LutFilterSpec, { name = "LUT filter"; });
    REGISTER_FILTER(LutFilterSpec);
    PROPERTY(LutFilterSpec, strength, { strength.name = "Strength"; });


    TYPE(SaturationFilterSpec, { name = "Saturation filter"; });
    REGISTER_FILTER(SaturationFilterSpec);
    PROPERTY(SaturationFilterSpec, multiplier, { multiplier.name = "Multiplier"; });


    TYPE(ContrastFilterSpec, { name = "Contrast filter"; });
    REGISTER_FILTER(ContrastFilterSpec);
    PROPERTY(ContrastFilterSpec, factor, { factor.name = "Factor"; });


    TYPE(ChannelMixerFilterSpec, { name = "Channel mixer filter"; });
    REGISTER_FILTER(ChannelMixerFilterSpec);

}
