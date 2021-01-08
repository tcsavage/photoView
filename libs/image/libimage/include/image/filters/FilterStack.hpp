#pragma once

#include <tuple>
#include <type_traits>

#include <image/Color.hpp>
#include <image/filters/Concepts.hpp>
#include <image/filters/Filter.hpp>

namespace image::filters {

    template <class T, bool Precalculate, FilterImpl... Impls>
    struct FilterStack {};

    template <class T, bool Precalculate, FilterImpl Impl0, FilterImpl... RestImpls>
    struct FilterStack<T, Precalculate, Impl0, RestImpls...> {
        Filter<Impl0, T, Precalculate> item;
        FilterStack<T, Precalculate, RestImpls...> next;
    };

    template <FilterImpl Target, class T, bool Precalculate, FilterImpl Impl0, FilterImpl... RestImpls>
    requires std::same_as<Target, Impl0>
    constexpr Filter<Target, T, Precalculate> &get(FilterStack<T, Precalculate, Impl0, RestImpls...> &fs) noexcept {
        return fs.item;
    }

    template <FilterImpl Target, class T, bool Precalculate, FilterImpl Impl0, FilterImpl... RestImpls>
    requires (!std::same_as<Target, Impl0>)
    constexpr Filter<Target, T, Precalculate> &get(FilterStack<T, Precalculate, Impl0, RestImpls...> &fs) noexcept {
        static_assert(sizeof...(RestImpls) > 0, "Filter implementation is not in this stack");
        return get<Target>(fs.next);
    }

}
