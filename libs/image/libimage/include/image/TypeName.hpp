#pragma once

#include <image/CoreTypes.hpp>

#define DEFTYPENAME(type) template <>   \
    class TypeName<type> {              \
    public:                             \
        static const char* get() {      \
            return #type;               \
        }                               \
    };

namespace image {

    template <typename T>
    class TypeName {
    public:
        static const char* get() {
            return typeid(T).name();
        }
    };

    DEFTYPENAME(U8)
    DEFTYPENAME(U16)
    DEFTYPENAME(U32)
    DEFTYPENAME(U64)

    DEFTYPENAME(I8)
    DEFTYPENAME(I16)
    DEFTYPENAME(I32)
    DEFTYPENAME(I64)

    //DEFTYPENAME(F16)
    DEFTYPENAME(F32)
    DEFTYPENAME(F64)

    DEFTYPENAME(String)
    DEFTYPENAME(StringView)
    DEFTYPENAME(Path)
    DEFTYPENAME(Instant)

}

#undef DEFTYPENAME
