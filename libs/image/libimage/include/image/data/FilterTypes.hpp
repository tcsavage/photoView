#pragma once

#include <image/Filters.hpp>
#include <image/data/Declare.hpp>
#include <image/data/PrimitiveTypes.hpp>

namespace image {

    TYPE(ExposureFilterSpec, {
        name = "Exposure filter";
    });

    PROPERTY(ExposureFilterSpec, exposureEvs, {
        exposureEvs.name = "Exposure EVs";
    });

}
