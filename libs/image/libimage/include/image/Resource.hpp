#pragma once

#include <optional>

#include <image/CoreTypes.hpp>
#include <image/Expected.hpp>
#include <image/ImageBuf.hpp>
#include <image/luts/Lattice3D.hpp>

namespace image {

    struct ResourceLoadError {
        Path filePath;
    };

    struct ImageResource {
        std::optional<Path> filePath;
        std::optional<ImageBuf<F32>> data;

        inline void setPath(Path path) noexcept {
            unload();
            filePath = path;
        }

        Expected<void, ResourceLoadError> load() noexcept;
        inline void unload() noexcept { data = std::nullopt; }

        explicit ImageResource() {}
        explicit ImageResource(Path filePath) : filePath(filePath) {}
    };

    struct LutResource {
        std::optional<Path> filePath;
        std::optional<luts::Lattice3D> data;

        inline void setPath(Path path) noexcept {
            unload();
            filePath = path;
        }

        Expected<void, ResourceLoadError> load() noexcept;
        inline void unload() noexcept { data = std::nullopt; }

        explicit LutResource() {}
        explicit LutResource(Path filePath) : filePath(filePath) {}
    };

}
