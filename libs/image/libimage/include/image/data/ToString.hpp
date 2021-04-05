#pragma once

#include <concepts>
#include <sstream>

#include <image/data/Interface.hpp>
#include <image/data/PrimitiveTypes.hpp>

namespace image {

    /**
     * @brief Interface definition.
     */
    struct ToString {
        virtual String toString(DynamicRef ref) const noexcept = 0;
    };

    /**
     * @brief ToString implementation which will work for any T.
     */
    template <DynamicType T>
    struct ToStringImpl final : public ToString {
        virtual String toString(DynamicRef ref) const noexcept override {
            std::stringstream ss;
            ss << "<" << ref.info().name;
            auto props = ref.properties();
            if (props.empty()) {
                ss << ">";
            } else {
                ss << ":";
                bool first = true;
                for (auto &&[ident, propRef] : props) {
                    if (!first) { ss << ","; };
                    first = false;
                    ss << " " << ident << " : ";
                    ss << propRef.info().ident;
                    ss << " = ";
                    ss << ifaceImpl<ToString>(propRef)->toString(propRef);
                }
                ss << ">";
            }
            return ss.str();
        }
    };

    /**
     * @brief ToString implementation for numeric types.
     */
    template <class T>
    requires std::integral<T> || std::floating_point<T>
    struct ToStringNumericImpl final : public ToString {
        virtual String toString(DynamicRef ref) const noexcept override {
            std::stringstream ss;
            ss << *ref.get<T>();
            return ss.str();
        }
    };

    /**
     * @brief ToString implementation for Path.
     */
    template <>
    struct ToStringImpl<Path> final : public ToString {
        virtual String toString(DynamicRef ref) const noexcept override { return ref.get<Path>()->string(); }
    };

    INTERFACE(ToString, I32, ToStringNumericImpl<I32>);
    INTERFACE(ToString, F32, ToStringNumericImpl<F32>);
    INTERFACE(ToString, Path, ToStringImpl<Path>);

}
