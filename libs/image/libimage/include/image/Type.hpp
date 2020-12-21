#pragma once

#include <type_traits>

#include <image/CoreTypes.hpp>
#include <image/Exception.hpp>
#include <image/TypeName.hpp>

namespace image {

    class Type {
    public:
        Type(StringView name, std::size_t size, std::size_t alignment);

        Type(const Type&) = delete;
        Type(Type&&) = delete;

        Type& operator=(const Type&) = delete;
        Type& operator=(Type&&) = delete;

        std::size_t size() const;
        std::size_t alignment() const;
        const String& name() const;

        bool operator==(const Type& rhs) const noexcept;
        bool operator!=(const Type& rhs) const noexcept;

    private:
        std::size_t _size { 0 };
        std::size_t _alignment { 0 };
        String _name;
    };

    class TypeRef {
    public:
        TypeRef(const Type& type);

        TypeRef(const TypeRef& other);
        TypeRef& operator=(const TypeRef& other);

        TypeRef(TypeRef&& other) noexcept;
        TypeRef& operator=(TypeRef&& other) noexcept;

        const Type* get() const;
        const Type& operator*() const;
        const Type* operator->() const;

        bool operator==(const TypeRef& rhs) const noexcept;
        bool operator!=(const TypeRef& rhs) const noexcept;

    private:
        const Type* _type;
    };

    template <typename T, typename Enable = void>
    class RepresentType {
    public:
        static TypeRef get() {
            throw Exception("Cannot represent type");
        }
    };

    template<typename T>
    class RepresentType<T, std::enable_if_t<std::is_arithmetic_v<T> || (std::is_standard_layout_v<T> && std::is_trivial_v<T>) || std::is_aggregate_v<T> || std::is_final_v<T>>> {
    public:
        static TypeRef get() {
            static Type ty { TypeName<T>::get(), sizeof(T), alignof(T) };
            return TypeRef(ty);
        }
    };

    template<>
    class RepresentType<String> {
    public:
        static TypeRef get() {
            static Type ty { TypeName<String>::get(), sizeof(String), alignof(String) };
            return TypeRef(ty);
        }
    };

    std::ostream& operator<<(std::ostream& os, const Type& fn);
    std::ostream& operator<<(std::ostream& os, const TypeRef& fn);

}
