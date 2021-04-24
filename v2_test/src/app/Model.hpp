#pragma once

#include <memory>
#include <vector>

#include <image/CoreTypes.hpp>

#include <app/Operations.hpp>

namespace image {

    struct AddFn;

    struct FunctionVisitor {
        virtual void visit(AddFn &fn) noexcept = 0;
    };

    struct AbstractFunction {
    public:
        virtual void accept(FunctionVisitor &visitor) noexcept = 0;
    };

    template <class Impl>
    struct FunctionBase : public AbstractFunction {
        virtual void accept(FunctionVisitor &visitor) noexcept override {
            visitor.visit(*static_cast<Impl *>(this));
        }
    };

    struct AddFn final : public FunctionBase<AddFn> {
        I32 operand { 0 };
    };

    struct Program {
        std::vector<std::shared_ptr<AbstractFunction>> functions;

        inline void addFunction(std::shared_ptr<AbstractFunction> function) noexcept {
            functions.push_back(function);
        }

        template <class T>
        std::shared_ptr<T> addFunction() noexcept {
            auto ptr = std::make_shared<T>();
            functions.push_back(ptr);
            return ptr;
        }
    };

}
