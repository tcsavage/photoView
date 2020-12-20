#include <image/luts/SimpleCube.hpp>

#include <cmath>
#include <iostream>
#include <tuple>
#include <utility>

#include <glm/geometric.hpp>

#include <image/Color.hpp>
#include <image/luts/Barycentric.hpp>

namespace image::luts {

    namespace {

        template <class V, class O>
        inline O closer2(V ref1, O out1, V ref2, O out2, V value) noexcept {
            if (glm::length(value - ref1) < glm::length(value - ref2)) {
                return out1;
            } else {
                return out2;
            }
        }

        template <class V, class O>
        inline O closer3(V ref1, O out1, V ref2, O out2, V ref3, O out3, V value) noexcept {
            if (closer2(ref1, true, ref2, false, value)) {
                return closer2(ref1, out1, ref3, out3, value);
            } else {
                return closer2(ref2, out2, ref3, out3, value);
            }
        }
        
    }

    ColorRGB SimpleCube::map(ColorRGB color) const {
        auto idx2 = closer3(inVerts[1], 1, inVerts[2], 2, inVerts[4], 4, color);
        ColorRGB v1, v2, v3, v4;
        ColorRGB c1, c2, c3, c4;
        std::size_t idx3;
        v1 = inVerts[0];
        v4 = inVerts[7];
        v2 = inVerts[idx2];
        c1 = outVerts[0];
        c4 = outVerts[7];
        c2 = outVerts[idx2];
        switch(idx2) {
        case 1:
            idx3 = closer2(inVerts[3], 3, inVerts[5], 5, color);
            break;
        case 2:
            idx3 = closer2(inVerts[3], 3, inVerts[6], 6, color);
            break;
        case 4:
            idx3 = closer2(inVerts[5], 5, inVerts[6], 6, color);
            break;
        default:
            __builtin_unreachable();
        }
        v3 = inVerts[idx3];
        c3 = outVerts[idx3];

        auto lam = barycentric(v1, v2, v3, v4, color);

        ColorRGB out { 0, 0, 0 };
        out += c1 * lam[0];
        out += c2 * lam[1];
        out += c3 * lam[2];
        out += c4 * lam[3];
        return out;
    }

}
