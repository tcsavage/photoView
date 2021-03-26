#include "Serialization.hpp"

#include <sstream>

#include <boost/property_tree/ptree.hpp>

namespace image::serialization {


    // Reading


    String ReadError::generateString() const noexcept {
        std::stringstream ss;
        ss << typeName << "." << key << " => " << reason;
        return ss.str();
    }

    ReadError::ReadError(String typeName, String key, String &&message) noexcept
      : typeName(typeName)
      , key(key)
      , reason(std::move(message)) {}

    Expected<void, ReadError> read(const ReadContext &, const pt::ptree &tree, glm::vec2 &vec) noexcept {
        if (auto x = tree.get_optional<F32>("x")) {
            vec.x = *x;
        } else {
            return Unexpected(ReadError { "Vec2", "x", "Missing" });
        }

        if (auto y = tree.get_optional<F32>("y")) {
            vec.y = *y;
        } else {
            return Unexpected(ReadError { "Vec2", "y", "Missing" });
        }

        return success;
    }

    Expected<void, ReadError>
    read(const ReadContext &ctx, const pt::ptree &tree, ImageResource &imageResource) noexcept {
        if (auto relPath = tree.get_optional<String>("path")) {
            // Make absolute path to resource.
            Path absPath = ctx.basePath / *relPath;
            imageResource.filePath = absPath;

            auto loadResult = imageResource.load();
            if (loadResult.hasError()) {
                return Unexpected(ReadError { "ImageResource", "path", "Failed to load image resource" });
            }
            return success;
        }
        return Unexpected(ReadError { "ImageResource", "path", "Missing" });
    }

    Expected<void, ReadError> read(const ReadContext &ctx, const pt::ptree &tree, LutResource &lutResource) noexcept {
        if (auto relPath = tree.get_optional<String>("path")) {
            // Make absolute path to resource.
            Path absPath = ctx.basePath / *relPath;
            lutResource.filePath = absPath;

            auto loadResult = lutResource.load();
            if (loadResult.hasError()) {
                return Unexpected(ReadError { "LutResource", "path", "Failed to load LUT resource" });
            }
            return success;
        }
        return success;
    }


    // Writing


    void write(const WriteContext &, pt::ptree &tree, const glm::vec2 vec) noexcept {
        tree.put("x", vec.x);
        tree.put("y", vec.y);
    }

    void write(const WriteContext &ctx, pt::ptree &tree, const ImageResource &imageResource) noexcept {
        if (imageResource.filePath) {
            auto relPath = std::filesystem::relative(*imageResource.filePath, ctx.basePath);
            tree.put("path", relPath.string());
        }
    }

    void write(const WriteContext &ctx, pt::ptree &tree, const LutResource &lutResource) noexcept {
        if (lutResource.filePath) {
            auto relPath = std::filesystem::relative(*lutResource.filePath, ctx.basePath);
            tree.put("path", relPath.string());
        }
    }

}
