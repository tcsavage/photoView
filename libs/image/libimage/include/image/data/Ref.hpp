#pragma once

#include <map>
#include <memory>

#include <image/Assert.hpp>
#include <image/data/Declare.hpp>

namespace image {

    class DynamicRef {
    public:
        template <DynamicType T>
        T &get() noexcept {
            ASSERT(typeInfo == &dynInfo<T>(), "Attempted to get a DynamicRef with wrong type");
            return *static_cast<T *>(ptr);
        }

        template <DynamicType T>
        T *ptr() noexcept {
            if (typeInfo != &dynInfo<T>()) {
                return nullptr;
            } else {
                return static_cast<T *>(ptr);
            }
        }

        template <class T>
        T *ptrUnchecked() noexcept {
            return static_cast<T *>(ptr);
        }

        DynamicRef operator[](StringView ident) noexcept {
            auto &propTypeInfo = (*typeInfo)[ident];
            return DynamicRef(propTypeInfo.get(ptr), propTypeInfo.propertyTypeInfo);
        }

        DynamicRef operator[](AbstractProperty *prop) noexcept {
            return DynamicRef(prop->get(ptr), prop->propertyTypeInfo);
        }


        std::map<StringView, DynamicRef> properties() noexcept {
            std::map<StringView, DynamicRef> out;
            for (auto &&[ident, propTypeInfo] : typeInfo->properties()) {
                out.insert({ ident, DynamicRef(propTypeInfo->get(ptr), propTypeInfo->propertyTypeInfo) });
            }
            return out;
        }

        const AbstractTypeInfo &info() const noexcept { return *typeInfo; }

        inline static DynamicRef makeUnchecked(void *ptr, AbstractTypeInfo *typeInfo) noexcept {
            return DynamicRef(ptr, typeInfo);
        }

        template <DynamicType T>
        DynamicRef(T *ptr) noexcept : DynamicRef(ptr, &dynInfo<T>()) {}

    private:
        void *ptr { nullptr };
        AbstractTypeInfo *typeInfo { nullptr };

        DynamicRef(void *ptr, AbstractTypeInfo *typeInfo) noexcept : ptr(ptr), typeInfo(typeInfo) {}
    };

    /**
     * @brief Convenience interface to make it easy to get a DynamicRef.
     */
    class Dynamic {
    public:
        virtual DynamicRef dynPtr() noexcept = 0;

        virtual ~Dynamic() noexcept {}
    };

}
