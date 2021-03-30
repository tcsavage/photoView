#pragma once

#include <vector>

#include <image/AllFilters.hpp>
#include <image/AllMaskGenerators.hpp>
#include <image/Composition.hpp>
#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/Resource.hpp>

namespace image::serialization {

    struct CompositionLoadError {
        Path path;
        String message;
    };

    void saveToFile(const Path &path, const image::Composition &comp) noexcept;

    image::Expected<image::Composition, CompositionLoadError>
    loadFromFile(const Path &path,
                 const image::FilterRegistry *filterRegistry,
                 const image::MaskGeneratorRegistry *maskGeneratorRegistry) noexcept;

    String encodeFilters(const std::vector<image::AbstractFilterSpec *> &filters) noexcept;
    std::vector<std::unique_ptr<image::AbstractFilterSpec>> decodeFilters(const String &encoded) noexcept;

    String encodeMask(const image::GeneratedMask *mask) noexcept;

}
