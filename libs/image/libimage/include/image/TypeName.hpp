#pragma once

#include <image/CoreTypes.hpp>

#define DEFTYPENAME(type) template <>                  \
    class TypeName<type> {                             \
    public:                                            \
        constexpr static StringView get() noexcept {   \
            return #type;                              \
        }                                              \
    }

namespace image {

    template <typename T>
    class TypeName {
    public:
        constexpr static StringView get() noexcept {
            return "UNKNOWN";
        }
    };

    DEFTYPENAME(bool);

    DEFTYPENAME(U8);
    DEFTYPENAME(U16);
    DEFTYPENAME(U32);
    DEFTYPENAME(U64);

    DEFTYPENAME(I8);
    DEFTYPENAME(I16);
    DEFTYPENAME(I32);
    DEFTYPENAME(I64);

    //DEFTYPENAME(F16);
    DEFTYPENAME(F32);
    DEFTYPENAME(F64);

    DEFTYPENAME(String);
    DEFTYPENAME(StringView);
    DEFTYPENAME(Path);
    DEFTYPENAME(Instant);

}

#undef DEFTYPENAME
