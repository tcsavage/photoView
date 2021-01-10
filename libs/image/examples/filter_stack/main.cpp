/**
 * @brief Simple example comparing FilterStack with a simple struct.
 * 
 * Useful to compare generated ASM.
 * As long as usingFilterStruct() and usingFilterStack() do the same thing, they
 * should generate the same ASM.
 */
#include <iostream>
#include <fstream>

#include <image/CoreTypes.hpp>
#include <image/filters/Exposure.hpp>
#include <image/filters/Filter.hpp>
#include <image/filters/FilterStack.hpp>
#include <image/filters/Lut.hpp>
#include <image/luts/TetrahedralInterpolator.hpp>

using namespace image;

struct FilterStruct {
    filters::Filter<F32, filters::Exposure<F32>, true> exposure;
    filters::Filter<F32, filters::Lut<luts::TetrahedralInterpolator, F32>, true> lut;
};

constexpr ColorRGB<F32> applyToColor(const ColorRGB<F32> &color, const FilterStruct &fs) noexcept {
    auto exposureOut = fs.exposure.applyToColor(color);
    auto lutOut = fs.lut.applyToColor(exposureOut);
    return lutOut;
}

FilterStruct filterStruct;

[[gnu::noinline]]
ColorRGB<F32> usingFilterStruct() {
    auto &f = filterStruct.exposure;
    f.impl.setExposure(0);
    auto &g = filterStruct.lut;
    g.setStrength(0.5);
    return applyToColor(ColorRGB<F32> { 0.1, 0.4, 0.8 }, filterStruct);
}

filters::FilterStack<
    F32,
    true,
    filters::Exposure<F32>,
    filters::Lut<luts::TetrahedralInterpolator, F32>
> filterStack;

[[gnu::noinline]]
ColorRGB<F32> usingFilterStack() {
    auto &f = filters::get<filters::Exposure<F32>>(filterStack);
    f.impl.setExposure(0);
    auto &g = filters::get<filters::Lut<luts::TetrahedralInterpolator, F32>>(filterStack);
    g.setStrength(0.5);
    return filters::applyToColor(ColorRGB<F32> { 0.1, 0.4, 0.8 }, filterStack);
}

int main(int, const char*[]) {
    usingFilterStruct();
    usingFilterStack();

    return 0;
}
