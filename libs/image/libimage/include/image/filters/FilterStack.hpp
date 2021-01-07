#pragma once

#include <tuple>
#include <type_traits>

#include <image/Color.hpp>
#include <image/filters/Concepts.hpp>
#include <image/filters/Filter.hpp>

namespace image::filters {

    template <class T, FilterImpl... Impls>
    struct FilterStack {};

    template <class T, FilterImpl Impl0, FilterImpl... RestImpls>
    struct FilterStack<T, Impl0, RestImpls...> {
        Filter<Impl0, T> item;
        FilterStack<T, RestImpls...> next;
    };

    template <FilterImpl Target, class T, FilterImpl Impl0, FilterImpl... RestImpls>
    requires std::same_as<Target, Impl0>
    constexpr Filter<Target, T> &get(FilterStack<T, Impl0, RestImpls...> &fs) noexcept {
        return fs.item;
    }

    template <FilterImpl Target, class T, FilterImpl Impl0, FilterImpl... RestImpls>
    requires (!std::same_as<Target, Impl0>)
    constexpr Filter<Target, T> &get(FilterStack<T, Impl0, RestImpls...> &fs) noexcept {
        static_assert(sizeof...(RestImpls) > 0, "Filter implementation is not in this stack");
        return get<Target>(fs.next);
    }

}
