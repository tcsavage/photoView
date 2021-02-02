#include <image/Resource.hpp>

#include <fstream>

#include <image/IO.hpp>
#include <image/luts/CubeFile.hpp>

namespace image {

    Expected<void, ResourceLoadError> ImageResource::load() noexcept {
        if (!filePath) {
            return success;
        }
        auto result = readImageBufFromFile<F32>(*filePath);
        if (result.hasValue()) {
            data = result.value();
            return success;
        } else {
            return Unexpected(ResourceLoadError { *filePath });
        }
    }

    Expected<void, ResourceLoadError> LutResource::load() noexcept {
        if (!filePath) {
            return success;
        }
        std::ifstream s;
        s.open(*filePath);
        if (!s.is_open()) {
            return Unexpected(ResourceLoadError { *filePath });
        }
        luts::CubeFile cube;
        s >> cube;
        data = cube.lattice();
        return success;
    }

}
