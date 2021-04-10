#pragma once

#include <concepts>
#include <sstream>
#include <variant>

#include <boost/property_tree/ptree.hpp>

#include <image/data/Interface.hpp>
#include <image/data/FilterTypes.hpp>
#include <image/data/PrimitiveTypes.hpp>

#include "Serialization.hpp"

namespace image::serialization {

    using Data = std::variant<String, I32, F32, pt::ptree>;

    void addData(pt::ptree &tree, const String &key, Data &&data) noexcept;

    struct Serialization {
        virtual Data encode(const WriteContext &, DynamicRef) const noexcept = 0;
    };

    void writeFilter(const WriteContext &ctx, pt::ptree &filterTree, AbstractFilterSpec &filterSpec) noexcept;

    /**
     * @brief Serialize directly into a primitive data type.
     */
    template <class T>
    struct TrivialSerialization final : public Serialization {
        virtual Data encode(const WriteContext &, DynamicRef ref) const noexcept override {
            auto data = ref.getUnchecked<T>();
            return *data;
        }
    };

    INTERFACE(Serialization, I32, TrivialSerialization<I32>);
    INTERFACE(Serialization, F32, TrivialSerialization<F32>);

    template <class T>
    struct SerializationViaString final : public Serialization {
        virtual Data encode(const WriteContext &, DynamicRef ref) const noexcept override {
            auto data = ref.getUnchecked<T>();
            std::stringstream ss;
            ss << *data;
            return ss.str();
        }
    };
    
    INTERFACE(Serialization, bool, SerializationViaString<bool>);

    /**
     * @brief Serializes a filter spec.
     */
    struct SerializationForFilter final : public Serialization {
        virtual Data encode(const WriteContext &ctx, DynamicRef ref) const noexcept override {
            pt::ptree filterTree;
            auto filter = ref.getUnchecked<AbstractFilterSpec>();
            filterTree.put("filter", filter->getMeta().id);
            filterTree.put("enabled", filter->isEnabled);
            auto props = ref.properties();
            if (!props.empty()) {
                pt::ptree optionsTree;
                for (auto &&[ident, propRef] : props) {
                    auto data = ifaceImpl<Serialization>(propRef)->encode(ctx, propRef);
                    addData(optionsTree, String(ident), std::move(data));
                }
                if (!optionsTree.empty()) {
                    filterTree.add_child("options", optionsTree);
                }
            }
            return filterTree;
        }
    };

    INTERFACE(Serialization, ExposureFilterSpec, SerializationForFilter);
    INTERFACE(Serialization, LutFilterSpec, SerializationForFilter);
    INTERFACE(Serialization, SaturationFilterSpec, SerializationForFilter);
    INTERFACE(Serialization, ContrastFilterSpec, SerializationForFilter);
    INTERFACE(Serialization, ChannelMixerFilterSpec, SerializationForFilter);

}
