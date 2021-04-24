#pragma once

#include <image/data/Declare.hpp>
#include <image/data/PrimitiveTypes.hpp>

#include <app/Model.hpp>
#include <app/Registry.hpp>

namespace image {

    using FunctionRegistry = StaticFactoryRegistry<AbstractFunction>;
    #define REGISTER_FUNCTION(type) PHOTO_VIEW_INTERNAL__REGISTER_FACTORY(FunctionRegistry, type, dynInfo<type>().ident)

    // Declare AddFn with the dynamic data API.
    TYPE(AddFn, {
        name = "Add";
    });
    PROPERTY(AddFn, operand, {});
    // PROPERTY_META(AddFn, operand, SemanticType, "number");
    // PROPERTY_META(AddFn, operand, ValueRange, { -10, 10 });

    // Register AddFn with the function registry.
    REGISTER_FUNCTION(AddFn);



    TYPE(Program, {
        name = "Program";
    });


    
    // Helper functions.
    inline AbstractTypeInfo &dynInfo(AbstractFunction &fn) noexcept {
        struct Visitor final : public FunctionVisitor {
            AbstractTypeInfo *typeInfo { nullptr };
            virtual void visit(AddFn &) noexcept override { typeInfo = &dynInfo<AddFn>(); }
        };

        auto visitor = Visitor {};
        fn.accept(visitor);
        return *visitor.typeInfo;
    }

}
