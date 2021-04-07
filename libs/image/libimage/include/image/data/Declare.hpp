#pragma once

#include <map>

#include <image/Assert.hpp>
#include <image/CoreTypes.hpp>
#include <image/PolyVal.hpp>
#include <image/data/Core.hpp>

namespace image {

    struct AbstractProperty {
        StringView ident;
        StringView name;
        StringView description;
        AbstractTypeInfo *propertyTypeInfo;
        AbstractTypeInfo *objectTypeInfo;

        virtual void *get(void *obj) const noexcept = 0;

        constexpr AbstractProperty(StringView ident,
                                   StringView name,
                                   StringView description,
                                   AbstractTypeInfo &propertyTypeInfo,
                                   AbstractTypeInfo &objectTypeInfo)
          : ident(ident)
          , name(name)
          , description(description)
          , propertyTypeInfo(&propertyTypeInfo)
          , objectTypeInfo(&objectTypeInfo) {}
    };

    struct AbstractTypeInfo {
        StringView ident;
        StringView name;
        StringView description;
        std::size_t size;
        std::size_t alignment;

        std::map<StringView, PolyVal<AbstractProperty>> *properties_ { nullptr };

        inline void ensurePropertiesMap() noexcept {
            if (!properties_) { properties_ = new std::map<StringView, PolyVal<AbstractProperty>>; }
        }

        template <class T>
        bool registerProperty(T &&property) noexcept {
            ensurePropertiesMap();
            properties_->emplace(property.ident, PolyVal<AbstractProperty> { std::move(property) });
            return true;
        }

        inline AbstractProperty &operator[](StringView ident) noexcept {
            ensurePropertiesMap();
            return *properties_->at(ident).ptr();
        }

        inline std::map<StringView, PolyVal<AbstractProperty>> &properties() noexcept {
            ensurePropertiesMap();
            return *properties_;
        }

        constexpr AbstractTypeInfo(StringView ident,
                                   StringView name,
                                   StringView description,
                                   std::size_t size,
                                   std::size_t alignment)
          : ident(ident)
          , name(name)
          , description(description)
          , size(size)
          , alignment(alignment) {}
    };

    template <class T>
    struct BaseTypeInfo : public AbstractTypeInfo {
        constexpr BaseTypeInfo(StringView ident) noexcept : AbstractTypeInfo(ident, ident, "", sizeof(T), alignof(T)) {}
    };

    template <class T>
    struct TypeInfo {};

    template <class T>
    struct TypeInfoTraits {};

    struct TypeRegistry {
        TypeRegistry() = delete;

        template <class T>
        static bool registerType() {
            TypeInfo<T> &ti = TypeInfoTraits<T>::desc;
            types.emplace(ti.ident, &ti);
            return true;
        }

        inline static std::map<StringView, AbstractTypeInfo *> types;
    };

    template <class T>
    concept DynamicType = requires(T a, TypeInfo<T> *ti) {
        typename TypeInfo<T>;
        typename TypeInfoTraits<T>;
        { ti }
        ->std::convertible_to<BaseTypeInfo<T> *>;
        { TypeInfoTraits<T>::desc }
        ->std::convertible_to<TypeInfo<T>>;
    };

    template <DynamicType T>
    constexpr TypeInfo<T> &dynInfo() noexcept {
        return TypeInfoTraits<T>::desc;
    }

    inline AbstractTypeInfo &dynInfo(StringView ident) noexcept { return *TypeRegistry::types.at(ident); }

    template <DynamicType ObjectType, DynamicType PropertyType>
    struct BaseProperty : public AbstractProperty {
        PropertyType ObjectType::*ptr;

        virtual void *get(void *obj) const noexcept override {
            return &(reinterpret_cast<ObjectType *>(obj)->*ptr);
        }

        constexpr BaseProperty(StringView ident, PropertyType ObjectType::*ptr)
          : AbstractProperty(ident,
                             ident,
                             "",
                             TypeInfoTraits<PropertyType>::desc,
                             TypeInfoTraits<ObjectType>::desc)
          , ptr(ptr) {}
    };

    template <auto memberPtr>
    struct TypedProperty {};

    template <class ObjectType, class PropertyType, PropertyType ObjectType::*memberPtr>
    struct TypedProperty<memberPtr> final : public BaseProperty<ObjectType, PropertyType> {
        template <class F>
        constexpr explicit TypedProperty(StringView ident, F &&f) noexcept
          : BaseProperty<ObjectType, PropertyType>(ident, memberPtr) {
            f(*this);
        }
    };

    template <DynamicType ObjectType, DynamicType PropertyType>
    struct BaseFunctionalProperty : public AbstractProperty {
        virtual PropertyType &get(ObjectType &obj) const noexcept = 0;

        virtual void *get(void *obj) const noexcept override {
            return &(this->get(*static_cast<ObjectType *>(obj)));
        };

        constexpr BaseFunctionalProperty(StringView ident)
          : AbstractProperty(ident,
                             ident,
                             "",
                             TypeInfoTraits<PropertyType>::desc,
                             TypeInfoTraits<ObjectType>::desc) {}
    };

    template <DynamicType PropertyType, DynamicType ObjectType>
    PropertyType &getProperty(const ObjectType &obj, StringView ident) noexcept {
        auto &prop = dynInfo<ObjectType>()[ident];
        ASSERT(prop.propertyTypeInfo == &dynInfo<PropertyType>(),
               "Dynamic property's type doesn't match what's expected");
        auto propVoidPtr = prop.get(&obj);
        return static_cast<PropertyType *>(propVoidPtr);
    }

}

// Magic boilerplate-removing macros. These are not strictly necessary and one could declare and
// register dynamic types without them, it's just easier this way.

#define DATA_API_INTERNAL__REGISTER_TYPE(type)                                \
    struct type##_registration {                                              \
        inline static bool isRegistered = TypeRegistry::registerType<type>(); \
    }

/**
 * @brief Declare and register `type` as dynamic.
 *
 * This must be declared first, before any properties are declared.
 * Declares a pair of structs: TypeInfo<type> and TypeInfoTraits<type> which you shouldn't need
 * to reference directly (use dynInfo instead).
 */
#define TYPE(type, init)                                                     \
    template <>                                                              \
    struct TypeInfo<type> final : public BaseTypeInfo<type> {                \
        constexpr TypeInfo<type>() noexcept : BaseTypeInfo<type>(#type) init \
    };                                                                       \
    template <>                                                              \
    struct TypeInfoTraits<type> {                                            \
        inline static TypeInfo<type> desc;                                   \
    };                                                                       \
    DATA_API_INTERNAL__REGISTER_TYPE(type)

/**
 * @brief Declare and register a property of a dynamic type.
 */
#define PROPERTY(objectType, propertyIdent, init)                                                        \
    struct Property_##objectType##_##propertyIdent##_registration {                                      \
        inline static bool isRegistered = TypeInfoTraits<objectType>::desc.registerProperty(             \
            TypedProperty<&objectType::propertyIdent> { #propertyIdent, [](auto &propertyIdent) init }); \
    }

/**
 * @brief Declare and register a functional property of a dynamic type.
 */
#define PROPERTY_FN(objectType, propertyType, propertyIdent, init, objBinder, getter)                            \
    struct FunctionalProperty_##objectType##_##propertyIdent                                                     \
      : public BaseFunctionalProperty<objectType, propertyType> {                                                \
        virtual propertyType &get(objectType &objBinder) const noexcept override getter                          \
        template <class F>                                                                                       \
        constexpr FunctionalProperty_##objectType##_##propertyIdent(StringView ident, F &&f)                     \
          : BaseFunctionalProperty<objectType, propertyType>(ident) {                                            \
            f(*this);                                                                                            \
        }                                                                                                        \
    };                                                                                                           \
    struct Property_##objectType##_##propertyIdent##_registration {                                              \
        inline static bool isRegistered = TypeInfoTraits<objectType>::desc.registerProperty(                     \
            FunctionalProperty_##objectType##_##propertyIdent { #propertyIdent, [](auto &propertyIdent) init }); \
    }
