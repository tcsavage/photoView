#pragma once

#include <concepts>

#include <image/Color.hpp>
#include <image/CoreTypes.hpp>

namespace image::filters {

    template <class F>
    concept FilterImpl = std::default_initializable<F>;

}
