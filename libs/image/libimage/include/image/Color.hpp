#pragma once

#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>

#include <image/CoreTypes.hpp>
#include <image/TypeName.hpp>

namespace image {

    using ColorComponentType = F32;

    struct ColorRGB final : public glm::vec<3, ColorComponentType, glm::defaultp> {
        constexpr ColorRGB() noexcept : glm::vec<3, ColorComponentType, glm::defaultp>(0) {}

        template <class... Args>
        constexpr ColorRGB(Args&& ... args) noexcept : glm::vec<3, ColorComponentType, glm::defaultp>(std::forward<Args>(args)...) {}
    };

}
