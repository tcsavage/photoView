#pragma once

#include <type_traits>

#include <image/CoreTypes.hpp>
#include <image/TypeName.hpp>

namespace image {

    class Type {
    public:
        constexpr Type(StringView name, std::size_t size, std::size_t alignment) noexcept
          : size_(size)
          , alignment_(alignment)
          , name_(name) {}

        Type(const Type&) = delete;
        Type(Type&&) = delete;

        Type& operator=(const Type&) = delete;
        Type& operator=(Type&&) = delete;

        constexpr std::size_t size() const noexcept { return size_; }
        constexpr std::size_t alignment() const noexcept { return alignment_; }
        constexpr StringView name() const noexcept { return name_; }

        constexpr bool operator==(const Type& rhs) const noexcept { return this == &rhs; }
        constexpr bool operator!=(const Type& rhs) const noexcept { return this != &rhs; }

    private:
        std::size_t size_ { 0 };
        std::size_t alignment_ { 0 };
        StringView name_;
    };

    class TypeRef {
    public:
        constexpr TypeRef(const Type& type) noexcept : type_(&type) {}

        constexpr TypeRef(const TypeRef& other) noexcept : type_(other.type_) {}
        constexpr TypeRef& operator=(const TypeRef& other) noexcept {
            type_ = other.type_;
            return *this;
        }

        constexpr TypeRef(TypeRef&& other) noexcept : type_(other.type_) {}
        constexpr TypeRef& operator=(TypeRef&& other) noexcept {
            type_ = other.type_;
            return *this;
        }

        constexpr const Type* get() const noexcept { return type_; }
        constexpr const Type& operator*() const noexcept { return *get(); }
        constexpr const Type* operator->() const noexcept { return get(); }

        constexpr bool operator==(const TypeRef& rhs) const noexcept { return type_ == rhs.type_; }
        constexpr bool operator!=(const TypeRef& rhs) const noexcept { return type_ != rhs.type_; }

    private:
        const Type* type_;
    };

    template <typename T, typename Enable = void>
    class RepresentType {
    public:
        constexpr static TypeRef get() {
            std::terminate();
        }
    };

    template<typename T>
    class RepresentType<T, std::enable_if_t<std::is_arithmetic_v<T> || (std::is_standard_layout_v<T> && std::is_trivial_v<T>) || std::is_aggregate_v<T> || std::is_final_v<T>>> {
    public:
        constexpr static Type type { TypeName<T>::get(), sizeof(String), alignof(String) };
        constexpr static TypeRef get() noexcept {
            return TypeRef(type);
        }
    };

    template<>
    class RepresentType<String> {
    public:
        constexpr static Type type { "String", sizeof(String), alignof(String) };
        constexpr static TypeRef get() noexcept {
            return TypeRef(type);
        }
    };

    std::ostream& operator<<(std::ostream& os, const Type& fn);
    std::ostream& operator<<(std::ostream& os, const TypeRef& fn);

}
