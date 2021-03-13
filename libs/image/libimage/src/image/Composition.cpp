#include <image/Composition.hpp>

#include <iostream>

namespace image {

    std::unique_ptr<AbstractFilterSpec> &Filters::addFilter(std::unique_ptr<AbstractFilterSpec> &&filter) noexcept {
        return filterSpecs.emplace_back(std::move(filter));
    }

    void Filters::removeFilters(int start, int numFilters) noexcept {
        auto it = filterSpecs.begin() + start;
        filterSpecs.erase(it, it + numFilters);
    }

    void Filters::rotateFilters(int src, int count, int dest) noexcept {
        auto srcIt = filterSpecs.begin() + src;
        auto destIt = filterSpecs.begin() + dest;
        std::rotate(srcIt, srcIt + count, destIt);
    }

    void dumpComp(Composition &comp) {
        std::cerr << "=== Composition dump ===\n";
        std::cerr << "Composition\n";
        for (auto &&layer : comp.layers) {
            std::cerr << "\tLayer\n";
            if (auto filters = layer->filters) {
                std::cerr << "\t\tFilters\n";
                for (auto &&filter : filters->filterSpecs) {
                    std::cerr << "\t\t\tFilter: " << filter->getMeta().name << " ("
                              << (filter->isEnabled ? "enabled" : "disabled") << ")\n";
                }
            }
            if (layer->mask) { std::cerr << "\t\tMask\n"; }
        }
        std::cerr << "========================\n";
    }

    Expected<Composition, CompositionCreationError> Composition::newFromPath(const Path &path) noexcept {
        Composition comp;
        comp.inputImage.setPath(path);
        auto loadResult = comp.inputImage.load();
        if (loadResult.hasError()) { return Unexpected(CompositionCreationError { path }); }
        comp.layers.push_back(std::make_shared<Layer>());
        return comp;
    }

}
