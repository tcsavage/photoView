#pragma once

#include <memory>

#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/Filters.hpp>
#include <image/Mask.hpp>
#include <image/Resource.hpp>

namespace image {

    struct CompositionCreationError {
        Path path;
    };

    struct Filters {
        std::vector<std::unique_ptr<AbstractFilterSpec>> filterSpecs;

        std::unique_ptr<AbstractFilterSpec> &addFilter(std::unique_ptr<AbstractFilterSpec> &&filter) noexcept;
        std::unique_ptr<AbstractFilterSpec> &addFilter(std::unique_ptr<AbstractFilterSpec> &&filter, int idx) noexcept;

        void removeFilters(int start, int numFilters) noexcept;
        void rotateFilters(int src, int count, int dest) noexcept;
    };

    struct Layer {
        std::shared_ptr<Filters> filters;
        std::shared_ptr<GeneratedMask> mask;
        bool isEnabled { true };

        Layer() : filters(std::make_shared<Filters>()) {}
    };

    struct Composition {
        std::vector<std::shared_ptr<Layer>> layers;
        ImageResource inputImage;

        static Expected<Composition, CompositionCreationError> newFromPath(const Path &path) noexcept;
    };

    void dumpComp(Composition &comp);

}
