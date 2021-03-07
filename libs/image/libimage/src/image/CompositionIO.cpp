#include <image/CompositionIO.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

namespace image {

    struct Context {
        Path basePath;
    };

    void write(const Context &, pt::ptree &tree, const glm::vec2 vec) {
        tree.add("", vec.x);
        tree.add("", vec.y);
    }

    void write(const Context &ctx, pt::ptree &tree, const LinearGradientMaskSpec &gen) noexcept {
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

    void write(const Context &ctx, pt::ptree &tree, const GeneratedMask &mask) noexcept {
        auto &meta = mask.generator()->getMeta();
        tree.put("generator", meta.id);
        pt::ptree subtree;
        if (meta.id == "maskGenerators.linearGradient") {
            write(ctx, subtree, *static_cast<LinearGradientMaskSpec *>(mask.generator()));
        }
        if (!subtree.empty()) { tree.put_child("options", subtree); }
    }

    void write(const Context &, pt::ptree &tree, const ExposureFilterSpec &filter) noexcept {
        tree.put("exposureEvs", filter.exposureEvs);
    }

    void write(const Context &ctx, pt::ptree &tree, const LutResource &lutResource) noexcept {
        if (lutResource.filePath) {
            auto relPath = std::filesystem::relative(*lutResource.filePath, ctx.basePath);
            tree.put("path", relPath.string());
        }
    }

    void write(const Context &ctx, pt::ptree &tree, const LutFilterSpec &filter) noexcept {
        pt::ptree subtree;
        write(ctx, subtree, filter.lut);
        tree.put_child("lut", subtree);
        tree.put("strength", filter.strength);
    }

    void write(const Context &ctx, pt::ptree &tree, const AbstractFilterSpec &filter) noexcept {
        auto &meta = filter.getMeta();
        tree.put("filter", meta.id);
        pt::ptree subtree;
        if (meta.id == "filters.exposure") {
            write(ctx, subtree, static_cast<const ExposureFilterSpec &>(filter));
        } else if (meta.id == "filters.lut") {
            write(ctx, subtree, static_cast<const LutFilterSpec &>(filter));
        }
        if (!subtree.empty()) { tree.put_child("options", subtree); }
    }

    void write(const Context &ctx, pt::ptree &tree, const ImageResource &imageResource) noexcept {
        if (imageResource.filePath) {
            auto relPath = std::filesystem::relative(*imageResource.filePath, ctx.basePath);
            tree.put("path", relPath.string());
        }
    }

    void write(const Context &ctx, pt::ptree &tree, const Layer &layer) noexcept {
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

    void write(const Context &ctx, pt::ptree &tree, const Composition &comp) noexcept {
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
        Context ctx { path.parent_path() };
        pt::ptree tree;
        tree.put("meta.version", 1);
        pt::ptree subtree;
        write(ctx, subtree, comp);
        tree.add_child("composition", subtree);
        pt::write_json(path, tree);
    }

}
