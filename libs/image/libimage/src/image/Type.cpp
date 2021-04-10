#include <image/Type.hpp>

namespace image {

    std::ostream& operator<<(std::ostream& os, const Type& ty) {
        return os << ty.name() << "@" << ty.size() << "," << ty.alignment();
    }

    std::ostream& operator<<(std::ostream& os, const TypeRef& ty) {
        return os << *ty;
    }

}
