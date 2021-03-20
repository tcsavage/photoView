#include "MaskGeneratorSerialization.hpp"

#include <boost/property_tree/ptree.hpp>

namespace image::serialization {

    // Helpers

    template <class T>
    T *cast(AbstractMaskGenerator *maskGen) noexcept {
        return static_cast<T *>(maskGen);
    }

    template <class T>
    const T *cast(const AbstractMaskGenerator *maskGen) noexcept {
        return static_cast<const T *>(maskGen);
    }

    // Luma

    void LumaMaskGeneratorSerialization::write(const WriteContext &,
                                               pt::ptree &,
                                               const AbstractMaskGenerator *) const noexcept {}

    Expected<void, ReadError> LumaMaskGeneratorSerialization::read(const ReadContext &,
                                                                   const pt::ptree &,
                                                                   AbstractMaskGenerator *) const noexcept {
        return success;
    }

    // Linear gradient

    void LinearGradientMaskGeneratorSerialization::write(const WriteContext &ctx,
                                                         pt::ptree &tree,
                                                         const AbstractMaskGenerator *maskGen) const noexcept {
        auto g = cast<LinearGradientMaskSpec>(maskGen);
        {
            pt::ptree subtree;
            serialization::write(ctx, subtree, g->from);
            tree.put_child("from", subtree);
        }
        {
            pt::ptree subtree;
            serialization::write(ctx, subtree, g->to);
            tree.put_child("to", subtree);
        }
    }

    Expected<void, ReadError>
    LinearGradientMaskGeneratorSerialization::read(const ReadContext &ctx,
                                                   const pt::ptree &tree,
                                                   AbstractMaskGenerator *maskGen) const noexcept {
        auto g = cast<LinearGradientMaskSpec>(maskGen);

        if (auto from = tree.get_child_optional("from")) {
            auto fromResult = serialization::read(ctx, *from, g->from);
            if (fromResult.hasError()) {
                return Unexpected(ReadError { "LinearGradientMaskSpec", "from", fromResult.error().generateString() });
            }
        }

        if (auto to = tree.get_child_optional("to")) {
            auto toResult = serialization::read(ctx, *to, g->to);
            if (toResult.hasError()) {
                return Unexpected(ReadError { "LinearGradientMaskSpec", "to", toResult.error().generateString() });
            }
        }

        return success;
    }

}
