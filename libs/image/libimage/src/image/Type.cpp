#include <image/Type.hpp>

namespace image {

    Type::Type(StringView name, std::size_t size, std::size_t alignment)
        : _size(size)
        , _alignment(alignment)
        , _name(name) {}

    std::size_t Type::size() const { return _size; }
    std::size_t Type::alignment() const { return _alignment; }
    const String& Type::name() const { return _name; }

    bool Type::operator==(const Type& rhs) const noexcept {
        return this == &rhs;
    }

    bool Type::operator!=(const Type& rhs) const noexcept {
        return this != &rhs;
    }

    TypeRef::TypeRef(const Type& type)
        : _type(&type) {}

    TypeRef::TypeRef(const TypeRef& other) : _type(other._type) {}

    TypeRef& TypeRef::operator=(const TypeRef& other) {
        _type = other._type;
        return *this;
    }

    TypeRef::TypeRef(TypeRef&& other) noexcept : _type(other._type) {}

    TypeRef& TypeRef::operator=(TypeRef&& other) noexcept {
        _type = other._type;
        return *this;
    }

    const Type* TypeRef::get() const { return _type; }
    const Type& TypeRef::operator*() const { return *get(); }
    const Type* TypeRef::operator->() const { return get(); }

    bool TypeRef::operator==(const TypeRef& rhs) const noexcept { return _type == rhs._type; }
    bool TypeRef::operator!=(const TypeRef& rhs) const noexcept { return _type != rhs._type; }

    std::ostream& operator<<(std::ostream& os, const Type& ty) {
        return os << ty.name() << "@" << ty.size() << "," << ty.alignment();
    }

    std::ostream& operator<<(std::ostream& os, const TypeRef& ty) {
        return os << *ty;
    }

}
