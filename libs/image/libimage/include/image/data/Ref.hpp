#pragma once

#include <image/data/Core.hpp>

namespace image {

    class DynamicRef {
    public:
        template <DynamicType T>
        T *get() noexcept {
            if (typeInfo != &dynInfo<T>()) {
                return nullptr;
            } else {
                return static_cast<T *>(ptr);
            }
        }

        DynamicRef operator[](StringView ident) noexcept {
            auto &propTypeInfo = (*typeInfo)[ident];
            return DynamicRef(propTypeInfo.getter(ptr), propTypeInfo.propertyTypeInfo);
        }

        std::map<StringView, DynamicRef> properties() noexcept {
            std::map<StringView, DynamicRef> out;
            for (auto &&[ident, propTypeInfo] : typeInfo->properties()) {
                out.insert({ ident, DynamicRef(propTypeInfo->getter(ptr), propTypeInfo->propertyTypeInfo) });
            }
            return out;
        }

        const AbstractTypeInfo &info() const noexcept { return *typeInfo; }

        template <DynamicType T>
        DynamicRef(T *ptr) noexcept : ptr(ptr)
                                    , typeInfo(&dynInfo<T>()) {}

    private:
        void *ptr { nullptr };
        AbstractTypeInfo *typeInfo { nullptr };

        DynamicRef(void *ptr, AbstractTypeInfo *typeInfo) noexcept : ptr(ptr), typeInfo(typeInfo) {}
    };

}
