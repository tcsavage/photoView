#pragma once

#include <glm/glm.hpp>

#include <image/CoreTypes.hpp>

namespace image::luts {

    using Cartesian = glm::vec3;
    using Barycentric = glm::vec4;

    constexpr bool allPositive(const Barycentric& coord) noexcept {
        return coord.x >= 0 && coord.y >= 0 && coord.z >= 0 && coord.w >= 0;
    }

    inline F32 scalarTripleProduct(const Cartesian& a, const Cartesian& b, const Cartesian& c) noexcept {
        return glm::dot(a, glm::cross(b, c));
    }

    Barycentric barycentric(const Cartesian& a, const Cartesian& b, const Cartesian& c, const Cartesian& d, const Cartesian& p) noexcept;

}
