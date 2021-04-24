#include <app/Operations.hpp>

namespace image {

    Op operator+(Op a, Op b) noexcept {
        if (a->tag() == OpTag::NoOp) { return b; }
        if (b->tag() == OpTag::NoOp) { return a; }
        if (a->tag() == OpTag::AddOp && b->tag() == OpTag::AddOp) {
            return addOp(castOp<AddOp>(a)->operand + castOp<AddOp>(b)->operand);
        }
        if (a->tag() == OpTag::MulOp && b->tag() == OpTag::MulOp) {
            return mulOp(castOp<MulOp>(a)->operand * castOp<MulOp>(b)->operand);
        }
        return seqOp(a, b);
    }

    std::ostream &operator<<(std::ostream &out, const Op &op) noexcept {
        if (op->tag() == OpTag::NoOp) {
            out << "noOp";
        } else if (op->tag() == OpTag::AddOp) {
            auto addOp = castOp<AddOp>(op);
            out << "addOp(" << addOp->operand << ")";
        } else if (op->tag() == OpTag::MulOp) {
            auto mulOp = castOp<MulOp>(op);
            out << "mulOp(" << mulOp->operand << ")";
        } else if (op->tag() == OpTag::SeqOp) {
            auto seqOp = castOp<SeqOp>(op);
            out << seqOp->first << " + " << seqOp->second;
        }
        return out;
    }

}
