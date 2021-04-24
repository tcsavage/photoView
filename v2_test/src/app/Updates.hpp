#pragma once

#include <image/CoreTypes.hpp>
#include <image/PolyVal.hpp>
#include <image/data/Ref.hpp>

#include <app/Model.hpp>

namespace image {

    struct AbstractUpdate {
        virtual void apply() const noexcept = 0;
    };

    using Update = PolyVal<AbstractUpdate>;

    template <class T>
    struct SetPropertyUpdate {
        DynamicRef ref;
        AbstractProperty *property;
        T value;

        virtual void apply() const noexcept override {
            DynamicRef ref = ref[property];
            T &data = *(ref.get<T>());
            data = value;
        }
    };

    using Id = U64;

    template <class T>
    struct Command {
        Id id;
        T command;
    };

    template <class T>
    struct SetPropertyValue {
        StringView propertyName;
        T value;

        void apply(DynamicRef ref) const noexcept { ref[propertyName].get() = operand; }
    };

}
