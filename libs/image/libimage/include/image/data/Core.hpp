#pragma once

#include <concepts>

namespace image {

    struct AbstractTypeInfo;

    template <class T> struct BaseTypeInfo;
    template <class T> struct TypeInfo;
    template <class T> struct TypeInfoTraits;

    template <class T>
    concept DynamicType = requires(T a, TypeInfo<T> *ti) {
        typename TypeInfo<T>;
        typename TypeInfoTraits<T>;
        { ti }
        ->std::convertible_to<BaseTypeInfo<T> *>;
        { TypeInfoTraits<T>::desc }
        ->std::convertible_to<TypeInfo<T>>;
    };

}
