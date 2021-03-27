#include "FiltersSerialization.hpp"

#include <sstream>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <image/AllFilters.hpp>

namespace image::serialization {

    String encodeFilters(const std::vector<image::AbstractFilterSpec *> &filters) noexcept {
        auto filterSerializationRegistry = makeFilterSerializationRegistry();
        WriteContext ctx { std::filesystem::current_path(), &filterSerializationRegistry, nullptr };
        pt::ptree tree;
        pt::ptree filtersTree;
        for (auto &&filter : filters) {
            pt::ptree filterTree;
            auto &meta = filter->getMeta();
            filterTree.put("filter", meta.id);
            filterTree.put("enabled", filter->isEnabled);
            pt::ptree subtree;
            auto serializerResult = filterSerializationRegistry.create(meta.id);
            if (serializerResult.hasValue()) { serializerResult.value()->write(ctx, subtree, filter); }
            if (!subtree.empty()) { filterTree.put_child("options", subtree); }
            filtersTree.push_back(std::make_pair("", filterTree));
        }
        tree.add_child("filters", filtersTree);
        std::stringstream ss;
        pt::write_json(ss, tree);
        return ss.str();
    }

    std::vector<std::unique_ptr<image::AbstractFilterSpec>> decodeFilters(const String &encoded) noexcept {
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

        std::vector<std::unique_ptr<image::AbstractFilterSpec>> filters;

        for (auto &&[key, filterTree] : tree.get_child("filters")) {
            if (auto filterName = filterTree.get_optional<String>("filter")) {
                // Create new filter implementation instance.
                auto createResult = ctx.filterRegistry->create(*filterName);
                if (createResult.hasError()) {
                    continue;
                }
                auto &filterSpec = *createResult;

                // Read filter-specific options.
                if (auto options = filterTree.get_child_optional("options")) {
                    auto filterSerializationCreateResult = ctx.filterSerializationRegistry->create(*filterName);
                    if (filterSerializationCreateResult.hasError()) {
                        continue;
                    }
                    auto &serialization = *filterSerializationCreateResult;
                    serialization->read(ctx, *options, filterSpec.get());
                }

                filterSpec->isEnabled = filterTree.get<bool>("enabled", true);

                filters.push_back(std::move(filterSpec));
            } else {
                continue;
            }
        }
        return filters;
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
        if (!subtree.empty()) { tree.put_child("lut", subtree); }
        tree.put("strength", f->strength);
    }

    Expected<void, ReadError> LutFilterSerialization::read(const ReadContext &ctx,
                                                           const pt::ptree &tree,
                                                           AbstractFilterSpec *filter) const noexcept {
        auto f = cast<LutFilterSpec>(filter);

        if (auto strength = tree.get_optional<F32>("strength")) { f->strength = *strength; }

        if (auto lutResource = tree.get_child_optional("lut")) {
            auto lutResult = serialization::read(ctx, *lutResource, f->lut);
            if (lutResult.hasError()) {
                return Unexpected(ReadError { "LutFilterSpec", "lut", lutResult.error().generateString() });
            }
            f->update();
        }

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
