#pragma once

#include <any>
#include <concepts>

namespace image {

    template <class I>
    class PolyVal final {
    public:
        using Interface = I;

        template <class Implementation>
        requires std::derived_from<Implementation, Interface>
        constexpr PolyVal(Implementation &&impl)
          : storage(std::forward<Implementation>(impl))
          , getter([](std::any &stor) -> Interface & { return std::any_cast<Implementation &>(stor); }) {}

        constexpr explicit PolyVal(const PolyVal &other) noexcept
          : storage(other.storage)
          , getter(other.getter) {}

        constexpr PolyVal &operator=(const PolyVal &other) noexcept {
            storage = other.storage;
            getter = other.getter;
            return *this;
        }

        constexpr explicit PolyVal(PolyVal &&other) noexcept
          : storage(std::move(other.storage))
          , getter(std::move(other.getter)) {}

        constexpr PolyVal &operator=(PolyVal &&other) noexcept {
            storage = std::move(other.storage);
            getter = std::move(other.getter);
            return *this;
        }

        Interface *operator->() { return &getter(storage); }

        Interface *ptr() noexcept { return &getter(storage); }

        template <class Implementation>
        Implementation *as() noexcept {
            return static_cast<Implementation *>(&getter(storage));
        }

    private:
        std::any storage;
        Interface &(*getter)(std::any &);
    };

}
