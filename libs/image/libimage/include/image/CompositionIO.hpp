#pragma once

#include <image/AllFilters.hpp>
#include <image/AllMaskGenerators.hpp>
#include <image/Composition.hpp>
#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>

namespace image {

    struct CompositionLoadError {
        Path path;
        String message;
    };

    void saveToFile(const Path &path, const Composition &comp) noexcept;

    Expected<Composition, CompositionLoadError>
    loadFromFile(const Path &path,
                 const FilterRegistry *filterRegistry,
                 const MaskGeneratorRegistry *maskGeneratorRegistry) noexcept;

}
