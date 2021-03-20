#include "FiltersSerialization.hpp"

#include <boost/property_tree/ptree.hpp>

namespace image::serialization {

    // Helpers

    template <class T>
    T *cast(AbstractFilterSpec *filter) noexcept {
        return static_cast<T *>(filter);
    }

    template <class T>
    const T *cast(const AbstractFilterSpec *filter) noexcept {
        return static_cast<const T *>(filter);
    }

    // Exposure

    void ExposureFilterSerialization::write(const WriteContext &,
                                            pt::ptree &tree,
                                            const AbstractFilterSpec *filter) const noexcept {
        auto f = cast<ExposureFilterSpec>(filter);
        tree.put("exposureEvs", f->exposureEvs);
    }

    Expected<void, ReadError> ExposureFilterSerialization::read(const ReadContext &,
                                                                const pt::ptree &tree,
                                                                AbstractFilterSpec *filter) const noexcept {
        auto f = cast<ExposureFilterSpec>(filter);
        if (auto exposureEvs = tree.get_optional<F32>("exposureEvs")) {
            f->exposureEvs = *exposureEvs;
            f->update();
        }

        return success;
    }

    // LUT

    void LutFilterSerialization::write(const WriteContext &ctx,
                                       pt::ptree &tree,
                                       const AbstractFilterSpec *filter) const noexcept {
        auto f = cast<LutFilterSpec>(filter);
        pt::ptree subtree;
        serialization::write(ctx, subtree, f->lut);
        tree.put_child("lut", subtree);
        tree.put("strength", f->strength);
    }

    Expected<void, ReadError> LutFilterSerialization::read(const ReadContext &ctx,
                                                           const pt::ptree &tree,
                                                           AbstractFilterSpec *filter) const noexcept {
        auto f = cast<LutFilterSpec>(filter);

        if (auto lutResource = tree.get_child_optional("lut")) {
            auto lutResult = serialization::read(ctx, *lutResource, f->lut);
            if (lutResult.hasError()) {
                return Unexpected(ReadError { "LutFilterSpec", "lut", lutResult.error().generateString() });
            }
        }

        if (auto strength = tree.get_optional<F32>("strength")) { f->strength = *strength; }

        f->update();

        return success;
    }

    // Saturation

    void SaturationFilterSerialization::write(const WriteContext &,
                                              pt::ptree &tree,
                                              const AbstractFilterSpec *filter) const noexcept {
        auto f = cast<SaturationFilterSpec>(filter);
        tree.put("multiplier", f->multiplier);
    }

    Expected<void, ReadError> SaturationFilterSerialization::read(const ReadContext &,
                                                                  const pt::ptree &tree,
                                                                  AbstractFilterSpec *filter) const noexcept {
        auto f = cast<SaturationFilterSpec>(filter);

        if (auto multiplier = tree.get_optional<F32>("multiplier")) { f->multiplier = *multiplier; }

        return success;
    }

    // Contrast

    void ContrastFilterSerialization::write(const WriteContext &,
                                            pt::ptree &tree,
                                            const AbstractFilterSpec *filter) const noexcept {
        auto f = cast<ContrastFilterSpec>(filter);
        tree.put("factor", f->factor);
    }

    Expected<void, ReadError> ContrastFilterSerialization::read(const ReadContext &,
                                                                const pt::ptree &tree,
                                                                AbstractFilterSpec *filter) const noexcept {
        auto f = cast<ContrastFilterSpec>(filter);

        if (auto factor = tree.get_optional<F32>("factor")) { f->factor = *factor; }

        return success;
    }

}
