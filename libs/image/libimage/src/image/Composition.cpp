#include <image/Composition.hpp>

namespace image {

    Expected<Composition, CompositionLoadError> Composition::newFromPath(const Path &path) noexcept {
        Composition comp;
        comp.inputImage.setPath(path);
        auto loadResult = comp.inputImage.load();
        if (loadResult.hasError()) { return Unexpected(CompositionLoadError { path }); }
        comp.look = std::make_shared<Look>();

        return comp;
    }

}
