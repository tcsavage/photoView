#pragma once

#include <memory>

#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/Look.hpp>
#include <image/Resource.hpp>

namespace image {

    struct CompositionLoadError {
        Path path;
    };

    struct Composition {
        ImageResource inputImage;
        std::shared_ptr<Look> look;

        static Expected<Composition, CompositionLoadError> newFromPath(const Path &path) noexcept;
    };

}
