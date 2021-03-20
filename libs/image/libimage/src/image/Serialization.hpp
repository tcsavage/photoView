#pragma once

#include <memory>

#include <boost/property_tree/ptree_fwd.hpp>

#include <image/AllFilters.hpp>
#include <image/AllMaskGenerators.hpp>
#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/Forward.hpp>
#include <image/Registry.hpp>
#include <image/Resource.hpp>

namespace image::serialization {

    namespace pt = boost::property_tree;

    struct FilterSerialization;

    using FilterSerializationRegistry = image::Registry<FilterSerialization, image::FilterMeta>;


    // Reading


    struct ReadError {
        String typeName;
        String key;
        String reason;

        String generateString() const noexcept;

        ReadError(String typeName, String key, String &&message) noexcept;
    };

    struct ReadContext {
        Path basePath;
        const FilterSerializationRegistry *filterSerializationRegistry { nullptr };
        const FilterRegistry *filterRegistry { nullptr };
        const MaskGeneratorRegistry *maskGeneratorRegistry { nullptr };
    };

    Expected<void, ReadError> read(const ReadContext &, const pt::ptree &tree, glm::vec2 &vec) noexcept;

    Expected<void, ReadError>
    read(const ReadContext &ctx, const pt::ptree &tree, ImageResource &imageResource) noexcept;

    Expected<void, ReadError> read(const ReadContext &ctx, const pt::ptree &tree, LutResource &lutResource) noexcept;


    // Writing


    struct WriteContext {
        Path basePath;
        const FilterSerializationRegistry *filterSerializationRegistry { nullptr };
    };

    void write(const WriteContext &, pt::ptree &tree, const glm::vec2 vec) noexcept;

    void write(const WriteContext &ctx, pt::ptree &tree, const ImageResource &imageResource) noexcept;

    void write(const WriteContext &ctx, pt::ptree &tree, const LutResource &lutResource) noexcept;

}
