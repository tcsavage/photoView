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
            , getter([](std::any &stor) -> Interface& { return std::any_cast<Implementation&>(stor); }) {}

        Interface *operator->() { return &getter(storage); }

        template <class Implementation>
        Implementation *as() noexcept {
            return static_cast<Implementation*>(&getter(storage));
        }

    private:
        std::any storage;
        Interface &(*getter)(std::any&);
    };

}
