#pragma once

#include <climits>
#include <type_traits>

#include <image/CoreTypes.hpp>

namespace image {

    inline U64 nextPowerOf2(U64 A) {
        A |= (A >> 1);
        A |= (A >> 2);
        A |= (A >> 4);
        A |= (A >> 8);
        A |= (A >> 16);
        A |= (A >> 32);
        return A + 1;
    }

    template <typename T>
    void ignore(T &&) {}

}


#ifndef NO_COPY_CTOR_ASSIGN
#define NO_COPY_CTOR_ASSIGN(type) type(const type&) = delete; type& operator=(const type&) = delete;
#else
#ifdef _MSC_VER
#pragma message "NO_COPY_CTOR_ASSIGN already defined. May result in unexpected behaviour"
#else
#warning "NO_COPY_CTOR_ASSIGN already defined. May result in unexpected behaviour"
#endif
#endif

#ifndef NO_MOVE_CTOR_ASSIGN
#define NO_MOVE_CTOR_ASSIGN(type) type(type&&) noexcept = delete; type& operator=(type&&) noexcept = delete;
#else
#ifdef _MSC_VER
#pragma message "NO_MOVE_CTOR_ASSIGN already defined. May result in unexpected behaviour"
#else
#warning "NO_MOVE_CTOR_ASSIGN already defined. May result in unexpected behaviour"
#endif
#endif

#ifndef NO_COPY_MOVE
#define NO_COPY_MOVE(type) NO_COPY_CTOR_ASSIGN(type) NO_MOVE_CTOR_ASSIGN(type)
#else
#ifdef _MSC_VER
#pragma message "NO_COPY_MOVE already defined. May result in unexpected behaviour"
#else
#warning "NO_COPY_MOVE already defined. May result in unexpected behaviour"
#endif
#endif
