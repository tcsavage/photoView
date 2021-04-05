#pragma once

#include <iostream>
#include <map>

#include <image/CoreTypes.hpp>
#include <image/data/Declare.hpp>
#include <image/data/Ref.hpp>

namespace image {

    template <class Interface>
    struct InterfaceRegistrations {
        InterfaceRegistrations() = delete;

        static std::map<StringView, PolyVal<Interface>> &impls() {
            // Static initialization order bites us if we make this an inline static variable, so
            // we make it a function instead.
            static std::map<StringView, PolyVal<Interface>> data;
            return data;
        };
        static Interface *impl(StringView ident) noexcept { return impls().at(ident).ptr(); }
        template <class T>
        requires std::derived_from<T, Interface> static bool registerImpl(StringView ident, T &&impl) noexcept {
            impls().emplace(ident, PolyVal<Interface> { std::move(impl) });
            return true;
        }
    };

    template <class Interface>
    Interface *ifaceImpl(const AbstractTypeInfo &typeInfo) noexcept {
        return InterfaceRegistrations<Interface>::impl(typeInfo.ident);
    }

    template <class Interface>
    Interface *ifaceImpl(DynamicRef ref) noexcept {
        return ifaceImpl<Interface>(ref.info());
    }

}

#define INTERFACE(interface, type, impl)                                                                      \
    struct type##_Impl_##interface##_registration {                                                           \
        inline static bool isRegistered =                                                                     \
            InterfaceRegistrations<interface>::registerImpl<impl>(TypeInfoTraits<type>::desc.ident, impl {}); \
    }
