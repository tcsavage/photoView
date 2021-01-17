#include <image/luts/TetrahedralInterpolator.hpp>

#include <utility>

#include <image/NDArray.hpp>

namespace image::luts {

    void TetrahedralInterpolator::load(const Lattice3D& lattice) noexcept {
        latticeSize = lattice.size;
        cubeTable = NDArray<SimpleCube>(Shape { latticeSize - 1, latticeSize - 1, latticeSize - 1 });
        auto sizef = static_cast<F32>(latticeSize - 1);
        for (std::size_t b = 0 ; b < latticeSize - 1 ; ++b) {
            for (std::size_t g = 0 ; g < latticeSize - 1 ; ++g) {
                for (std::size_t r = 0 ; r < latticeSize - 1 ; ++r) {
                    auto& cube = cubeTable.at(r, g, b);
                    cube.inVerts = {
                        static_cast<glm::vec3>(glm::u64vec3(r, g, b)) / sizef,
                        static_cast<glm::vec3>(glm::u64vec3(r + 1, g, b)) / sizef,
                        static_cast<glm::vec3>(glm::u64vec3(r, g + 1, b)) / sizef,
                        static_cast<glm::vec3>(glm::u64vec3(r + 1, g + 1, b)) / sizef,
                        static_cast<glm::vec3>(glm::u64vec3(r, g, b + 1)) / sizef,
                        static_cast<glm::vec3>(glm::u64vec3(r + 1, g, b + 1)) / sizef,
                        static_cast<glm::vec3>(glm::u64vec3(r , g + 1, b + 1)) / sizef,
                        static_cast<glm::vec3>(glm::u64vec3(r + 1, g + 1, b + 1)) / sizef,
                    };
                    cube.outVerts = {
                        lattice.table.at(r, g, b),
                        lattice.table.at(r + 1, g, b),
                        lattice.table.at(r, g + 1, b),
                        lattice.table.at(r + 1, g + 1, b),
                        lattice.table.at(r, g, b + 1),
                        lattice.table.at(r + 1, g, b + 1),
                        lattice.table.at(r, g + 1, b + 1),
                        lattice.table.at(r + 1, g + 1, b + 1)
                    };
                }
            }
        }
    }

    const SimpleCube &TetrahedralInterpolator::findCube(ColorRGB<F32> color) const {
        auto coordf = color * static_cast<F32>(latticeSize - 1); // Cooridnate scaled-up to size of LUT.
        auto baseCoord = glm::clamp(static_cast<glm::u64vec3>(glm::floor(coordf)), static_cast<std::size_t>(0), latticeSize - 2); // Truncate to int and clamp to find index of cube.
        return cubeTable.at(baseCoord.r, baseCoord.g, baseCoord.b);
    }

    ColorRGB<F32> TetrahedralInterpolator::map(const ColorRGB<F32>& color) const noexcept {
        if (glm::length(color - inCache) < 0.01f) {
            return outCache;
        }
        const SimpleCube &cube = findCube(color);
        auto out = cube.map(color);
        inCache = color;
        outCache = out;
        return out;
    }

}
