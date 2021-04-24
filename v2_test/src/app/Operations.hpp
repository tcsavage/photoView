#pragma once

#include <memory>

#include <image/CoreTypes.hpp>

namespace image {

    struct IOp;

    enum class OpTag { NoOp, AddOp, MulOp, SeqOp };

    using Op = std::shared_ptr<IOp>;

    struct IOp {
        virtual OpTag tag() const noexcept = 0;

        virtual void apply(int &val) const noexcept = 0;
    };

    template <class T, class... Args>
    Op makeOp(Args &&... args) noexcept {
        return std::make_shared<T, Args...>(std::forward<Args>(args)...);
    }

    template <class T>
    std::shared_ptr<T> castOp(Op op) noexcept {
        if (op->tag() == T::TAG) { return std::static_pointer_cast<T>(op); }
        return nullptr;
    }

    template <OpTag Tag>
    struct TaggedOp : public IOp {
        constexpr static OpTag TAG { Tag };
        virtual OpTag tag() const noexcept override { return Tag; }
    };

    struct NoOp final : TaggedOp<OpTag::NoOp> {
        virtual void apply(int &) const noexcept {}
    };

    inline static auto noOp = makeOp<NoOp>();

    struct AddOp final : TaggedOp<OpTag::AddOp> {
        int operand { 0 };

        virtual void apply(int &val) const noexcept { val += operand; }

        constexpr explicit AddOp(int operand) noexcept : operand(operand) {}
    };

    inline Op addOp(int operand) { return makeOp<AddOp>(operand); }

    struct MulOp final : TaggedOp<OpTag::MulOp> {
        int operand { 0 };

        virtual void apply(int &val) const noexcept { val *= operand; }

        constexpr explicit MulOp(int operand) noexcept : operand(operand) {}
    };

    inline Op mulOp(int operand) { return makeOp<MulOp>(operand); }

    struct SeqOp final : TaggedOp<OpTag::SeqOp> {
        Op first;
        Op second;

        virtual void apply(int &val) const noexcept {
            first->apply(val);
            second->apply(val);
        }

        SeqOp(Op first, Op second) noexcept : first(first), second(second) {}
    };

    inline Op seqOp(Op first, Op second) { return makeOp<SeqOp>(first, second); }

    Op operator+(Op a, Op b) noexcept;

    std::ostream &operator<<(std::ostream &out, const Op &op) noexcept;

}
