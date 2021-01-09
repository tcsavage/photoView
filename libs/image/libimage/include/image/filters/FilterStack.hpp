#pragma once

#include <tuple>
#include <type_traits>

#include <image/Color.hpp>
#include <image/filters/Concepts.hpp>
#include <image/filters/Filter.hpp>

namespace image::filters {

    template <class T, bool Precalculate, FilterImpl<T>... Impls>
    struct FilterStack {};

    template <class T, bool Precalculate, FilterImpl<T> Impl0, FilterImpl<T>... RestImpls>
    struct FilterStack<T, Precalculate, Impl0, RestImpls...> {
        Filter<T, Impl0, Precalculate> item;
        FilterStack<T, Precalculate, RestImpls...> next;
    };

    template <class Target, class T, bool Precalculate, FilterImpl<T> Impl0, FilterImpl<T>... RestImpls>
    requires FilterImpl<Target, T> && std::same_as<Target, Impl0>
    constexpr Filter<T, Target, Precalculate> &get(FilterStack<T, Precalculate, Impl0, RestImpls...> &fs) noexcept {
        return fs.item;
    }

    template <class Target, class T, bool Precalculate, FilterImpl<T> Impl0, FilterImpl<T>... RestImpls>
    requires FilterImpl<Target, T> && (!std::same_as<Target, Impl0>)
    constexpr Filter<T, Target, Precalculate> &get(FilterStack<T, Precalculate, Impl0, RestImpls...> &fs) noexcept {
        static_assert(sizeof...(RestImpls) > 0, "Filter implementation is not in this stack");
        return get<Target>(fs.next);
    }

    template <class T, bool Precalculate, FilterImpl<T> Impl0, FilterImpl<T>... RestImpls>
    [[gnu::hot, gnu::flatten]]
    constexpr ColorRGB<T> applyToColor(const ColorRGB<T> &color, const FilterStack<T, Precalculate, Impl0, RestImpls...> &fs) noexcept {
        auto out = fs.item.applyToColor(color);
        if constexpr(sizeof...(RestImpls) > 0) {
            return applyToColor(out, fs.next);
        } else {
            return out;
        }
    }

}
