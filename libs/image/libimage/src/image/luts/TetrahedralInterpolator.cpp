#include <image/luts/TetrahedralInterpolator.hpp>

#include <utility>

#include <image/NDArray.hpp>

namespace image::luts {

    void TetrahedralInterpolator::load(const Lut& lut) noexcept {
        lutSize = lut.size;
        cubeTable = NDArray<SimpleCube>(Shape { lutSize - 1, lutSize - 1, lutSize - 1 });
        auto sizef = static_cast<F32>(lutSize - 1);
        for (std::size_t b = 0 ; b < lutSize - 1 ; ++b) {
            for (std::size_t g = 0 ; g < lutSize - 1 ; ++g) {
                for (std::size_t r = 0 ; r < lutSize - 1 ; ++r) {
                    auto& cube = cubeTable.at(Shape{ r, g, b });
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
                        lut.table.at(Shape {r, g, b}),
                        lut.table.at(Shape {r + 1, g, b}),
                        lut.table.at(Shape {r, g + 1, b}),
                        lut.table.at(Shape {r + 1, g + 1, b}),
                        lut.table.at(Shape {r, g, b + 1}),
                        lut.table.at(Shape {r + 1, g, b + 1}),
                        lut.table.at(Shape {r, g + 1, b + 1}),
                        lut.table.at(Shape {r + 1, g + 1, b + 1})
                    };
                }
            }
        }
    }

    const SimpleCube &TetrahedralInterpolator::findCube(ColorRGB<F32> color) const {
        auto coordf = color * static_cast<F32>(lutSize - 1); // Cooridnate scaled-up to size of LUT.
        auto baseCoord = glm::clamp(static_cast<glm::u64vec3>(glm::floor(coordf)), static_cast<std::size_t>(0), lutSize - 2); // Truncate to int and clamp to find index of cube.
        return cubeTable.at(Shape {baseCoord.r, baseCoord.g, baseCoord.b});
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
