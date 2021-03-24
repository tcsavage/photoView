#include "FiltersSerialization.hpp"

#include <sstream>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <image/AllFilters.hpp>

namespace image::serialization {

    String encodeFilter(const image::AbstractFilterSpec &filter) noexcept {
        auto filterSerializationRegistry = makeFilterSerializationRegistry();
        WriteContext ctx { std::filesystem::current_path(), &filterSerializationRegistry, nullptr };
        pt::ptree tree;
        auto &meta = filter.getMeta();
        tree.put("filter", meta.id);
        tree.put("enabled", filter.isEnabled);
        pt::ptree subtree;
        auto serializerResult = filterSerializationRegistry.create(meta.id);
        if (serializerResult.hasValue()) { serializerResult.value()->write(ctx, subtree, &filter); }
        if (!subtree.empty()) { tree.put_child("options", subtree); }
        std::stringstream ss;
        pt::write_json(ss, tree);
        return ss.str();
    }

    std::unique_ptr<image::AbstractFilterSpec> decodeFilter(const String &encoded) noexcept {
        auto filterSerializationRegistry = makeFilterSerializationRegistry();
        auto filterRegistry = makeFilterRegistry();
        ReadContext ctx { std::filesystem::current_path(),
                          &filterSerializationRegistry,
                          nullptr,
                          &filterRegistry,
                          nullptr };
        
        pt::ptree tree;
        std::stringstream ss { encoded };
        pt::read_json(ss, tree);

        if (auto filterName = tree.get_optional<String>("filter")) {
            // Create new filter implementation instance.
            auto createResult = ctx.filterRegistry->create(*filterName);
            if (createResult.hasError()) {
                return nullptr;
            }
            auto &filterSpec = *createResult;

            // Read filter-specific options.
            if (auto options = tree.get_child_optional("options")) {
                auto filterSerializationCreateResult = ctx.filterSerializationRegistry->create(*filterName);
                if (filterSerializationCreateResult.hasError()) {
                    return nullptr;
                }
                auto &serialization = *filterSerializationCreateResult;
                serialization->read(ctx, *options, filterSpec.get());
            }

            filterSpec->isEnabled = tree.get<bool>("enabled", true);

            return std::move(filterSpec);
        } else {
            return nullptr;
        }
    }

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
