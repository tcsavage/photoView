#include <image/CompositionSerialization.hpp>

#include <memory>
#include <sstream>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "FiltersSerialization.hpp"
#include "Serialization.hpp"

namespace pt = boost::property_tree;

using namespace image;

namespace image::serialization {

    void write(const WriteContext &ctx, pt::ptree &tree, const LinearGradientMaskSpec &gen) noexcept {
        {
            pt::ptree subtree;
            write(ctx, subtree, gen.from);
            tree.put_child("from", subtree);
        }
        {
            pt::ptree subtree;
            write(ctx, subtree, gen.to);
            tree.put_child("to", subtree);
        }
    }

    void write(const WriteContext &ctx, pt::ptree &tree, const GeneratedMask &mask) noexcept {
        auto &meta = mask.generator()->getMeta();
        tree.put("generator", meta.id);
        pt::ptree subtree;
        if (meta.id == "maskGenerators.linearGradient") {
            write(ctx, subtree, *static_cast<LinearGradientMaskSpec *>(mask.generator()));
        }
        if (!subtree.empty()) { tree.put_child("options", subtree); }
    }

    void write(const WriteContext &, pt::ptree &tree, const ExposureFilterSpec &filter) noexcept {
        tree.put("exposureEvs", filter.exposureEvs);
    }

    void write(const WriteContext &ctx, pt::ptree &tree, const LutFilterSpec &filter) noexcept {
        pt::ptree subtree;
        write(ctx, subtree, filter.lut);
        tree.put_child("lut", subtree);
        tree.put("strength", filter.strength);
    }

    void write(const WriteContext &, pt::ptree &tree, const SaturationFilterSpec &filter) noexcept {
        tree.put("multiplier", filter.multiplier);
    }

    void write(const WriteContext &, pt::ptree &tree, const ContrastFilterSpec &filter) noexcept {
        tree.put("factor", filter.factor);
    }

    void write(const WriteContext &ctx, pt::ptree &tree, const AbstractFilterSpec &filter) noexcept {
        auto &meta = filter.getMeta();
        tree.put("filter", meta.id);
        pt::ptree subtree;
        if (meta.id == "filters.exposure") {
            write(ctx, subtree, static_cast<const ExposureFilterSpec &>(filter));
        } else if (meta.id == "filters.lut") {
            write(ctx, subtree, static_cast<const LutFilterSpec &>(filter));
        } else if (meta.id == "filters.saturation") {
            write(ctx, subtree, static_cast<const SaturationFilterSpec &>(filter));
        } else if (meta.id == "filters.contrast") {
            write(ctx, subtree, static_cast<const ContrastFilterSpec &>(filter));
        }
        if (!subtree.empty()) { tree.put_child("options", subtree); }
    }

    void write(const WriteContext &ctx, pt::ptree &tree, const Layer &layer) noexcept {
        for (auto &&filterSpec : layer.filters->filterSpecs) {
            pt::ptree subtree;
            write(ctx, subtree, *filterSpec);
            tree.add_child("filters.", subtree);
        }
        if (layer.mask) {
            pt::ptree subtree;
            write(ctx, subtree, *layer.mask);
            tree.put_child("mask", subtree);
        }
    }

    void write(const WriteContext &ctx, pt::ptree &tree, const Composition &comp) noexcept {
        {
            pt::ptree subtree;
            write(ctx, subtree, comp.inputImage);
            tree.add_child("inputImage", subtree);
        }
        for (auto &&layer : comp.layers) {
            pt::ptree subtree;
            write(ctx, subtree, *layer);
            tree.add_child("layers.", subtree);
        }
    }

    void saveToFile(const Path &path, const Composition &comp) noexcept {
        auto filterSerializationRegistry = makeFilterSerializationRegistry();
        WriteContext ctx { path.parent_path(), &filterSerializationRegistry };
        pt::ptree tree;
        tree.put("meta.version", 1);
        pt::ptree subtree;
        write(ctx, subtree, comp);
        tree.add_child("composition", subtree);
        pt::write_json(path, tree);
    }



    Expected<void, ReadError> read(const ReadContext &ctx, const pt::ptree &tree, Filters &filters) noexcept {
        for (auto &&[path, filterTree] : tree) {
            if (auto filterName = filterTree.get_optional<String>("filter")) {
                // Check that we have a filter registry.
                if (!ctx.filterRegistry) {
                    return Unexpected(ReadError { "Filters", "*", "No filter registry set" });
                }

                // Check that we have a filter serialization registry.
                if (!ctx.filterSerializationRegistry) {
                    return Unexpected(ReadError { "Filters", "*", "No filter serialization registry set" });
                }

                // Create new filter implementation instance.
                auto createResult = ctx.filterRegistry->create(*filterName);
                if (createResult.hasError()) {
                    std::stringstream keySs;
                    keySs << "[" << filters.filterSpecs.size() << "]";
                    std::stringstream messageSs;
                    messageSs << "No filter named '" << *filterName << "' could be found in the registry";
                    return Unexpected(ReadError { "Filters", keySs.str(), messageSs.str() });
                }
                auto &filterSpec = *createResult;

                // Read filter-specific options.
                if (auto options = filterTree.get_child_optional("options")) {
                    auto filterSerializationCreateResult = ctx.filterSerializationRegistry->create(*filterName);
                    if (filterSerializationCreateResult.hasError()) {
                        std::stringstream keySs;
                        keySs << "[" << filters.filterSpecs.size() << "]";
                        std::stringstream messageSs;
                        messageSs << "Filter named '" << *filterName << "' has options but no serialisation handler";
                        return Unexpected(ReadError { "Filters", keySs.str(), messageSs.str() });
                    }
                    auto &serialization = *filterSerializationCreateResult;
                    serialization->read(ctx, *options, filterSpec.get());
                }

                filters.filterSpecs.emplace_back(std::move(filterSpec));
            } else {
                std::stringstream ss;
                ss << "[" << filters.filterSpecs.size() << "]";
                return Unexpected(ReadError { "Filters", ss.str(), "No filter name" });
            }
        }
        return success;
    }

    Expected<void, ReadError>
    read(const ReadContext &ctx, const pt::ptree &tree, LinearGradientMaskSpec &spec) noexcept {
        if (auto from = tree.get_child_optional("from")) {
            auto fromResult = read(ctx, *from, spec.from);
            if (fromResult.hasError()) {
                return Unexpected(
                    ReadError { "LinearGradientMaskSpec", "from", fromResult.error().generateString() });
            }
        }

        if (auto to = tree.get_child_optional("to")) {
            auto toResult = read(ctx, *to, spec.to);
            if (toResult.hasError()) {
                return Unexpected(
                    ReadError { "LinearGradientMaskSpec", "to", toResult.error().generateString() });
            }
        }

        return success;
    }

    Expected<void, ReadError> read(const ReadContext &ctx, const pt::ptree &tree, GeneratedMask &mask) noexcept {
        std::cerr << "Reading mask\n";
        if (auto generatorId = tree.get_optional<String>("generator")) {
            std::cerr << "Mask generator is " << *generatorId << "\n";
            // Check that we have a mask generator registry.
            if (!ctx.filterRegistry) {
                return Unexpected(ReadError { "GeneratedMask", "*", "No mask generator registry set" });
            }

            // Create new generator implementation instance.
            auto createResult = ctx.maskGeneratorRegistry->create(*generatorId);
            if (createResult.hasError()) {
                std::stringstream messageSs;
                messageSs << "No mask generator named '" << *generatorId << "' could be found in the registry";
                return Unexpected(ReadError { "GeneratedMask", "generator", messageSs.str() });
            }
            auto &generator = *createResult;

            std::cerr << "Created generator\n";

            // Read generator-specific options.
            if (auto options = tree.get_child_optional("options")) {
                if (generator->getMeta().id == "maskGenerators.linearGradient") {
                    auto optionsResult = read(ctx, *options, static_cast<LinearGradientMaskSpec &>(*generator));
                    if (optionsResult.hasError()) {
                        return Unexpected(ReadError {
                            "GeneratedMask", "options", optionsResult.error().generateString() });
                    }
                }
            }

            std::cerr << "Inserting into generated mask\n";

            mask.setGenerator(std::move(generator));
        } else {
            return Unexpected(ReadError { "GeneratedMask", "generator", "Missing" });
        }

        return success;
    }

    Expected<void, ReadError> read(const ReadContext &ctx, const pt::ptree &tree, Layer &layer) noexcept {
        if (auto filters = tree.get_child_optional("filters")) {
            if (!layer.filters) {
                return Unexpected(ReadError { "Layer", "filters", "Filters not yet created" });
            }
            auto filtersResult = read(ctx, *filters, *layer.filters);
            if (filtersResult.hasError()) {
                return Unexpected(ReadError { "Layer", "filters", filtersResult.error().generateString() });
            }
        }

        std::cerr << "Read layer filters\n";

        if (auto mask = tree.get_child_optional("mask")) {
            std::cerr << "Mask present\n";
            layer.mask = std::make_shared<GeneratedMask>();
            auto maskResult = read(ctx, *mask, *layer.mask);
            if (maskResult.hasError()) {
                return Unexpected(ReadError { "Layer", "mask", maskResult.error().generateString() });
            }
        }

        std::cerr << "Layer complete\n";

        return success;
    }

    Expected<void, ReadError> read(const ReadContext &ctx, const pt::ptree &tree, Composition &comp) noexcept {
        auto &image = tree.get_child("inputImage");
        auto imageResult = read(ctx, image, comp.inputImage);
        if (imageResult.hasError()) {
            return Unexpected(
                ReadError { "Composition", "inputImage", imageResult.error().generateString() });
        }

        // Read layers.
        if (auto layers = tree.get_child_optional("layers")) {
            for (auto &&[path, layerTree] : *layers) {
                // Read layer data.
                auto layer = std::make_shared<Layer>();
                auto layerResult = read(ctx, layerTree, *layer);
                if (layerResult.hasError()) {
                    std::stringstream ss;
                    ss << "layers[" << comp.layers.size() << "]";
                    return Unexpected(
                        ReadError { "Composition", ss.str(), layerResult.error().generateString() });
                }

                // Update the generated mask
                if (layer->mask) { layer->mask->update(*comp.inputImage.data); }

                comp.layers.push_back(layer);
            }
        }

        return success;
    }

    Expected<Composition, CompositionLoadError>
    loadFromFile(const Path &path,
                 const FilterRegistry *filterRegistry,
                 const MaskGeneratorRegistry *maskGeneratorRegistry) noexcept {
        auto filterSerializationRegistry = makeFilterSerializationRegistry();
        ReadContext ctx { path.parent_path(), &filterSerializationRegistry, filterRegistry, maskGeneratorRegistry };
        pt::ptree tree;
        pt::read_json(path, tree);

        if (!tree.get_child_optional("meta")) {
            return Unexpected(CompositionLoadError { path, "Unsupported file format (no meta node)" });
        }

        if (tree.get<int>("meta.version", -1) != 1) {
            return Unexpected(CompositionLoadError { path, "Unsupported file format version" });
        }

        if (!tree.get_child_optional("composition")) {
            return Unexpected(CompositionLoadError { path, "Unsupported file format (no composition node)" });
        }

        Composition comp;
        auto readResult = read(ctx, tree.get_child("composition"), comp);
        if (readResult.hasError()) {
            auto &err = readResult.error();
            std::stringstream ss;
            ss << "Parse error: " << err.generateString() << ".";
            return Unexpected(CompositionLoadError { path, ss.str() });
        }

        return comp;
    }
}