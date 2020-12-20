#include <image/luts/Barycentric.hpp>

namespace image::luts {

    Barycentric barycentric(const Cartesian& a, const Cartesian& b, const Cartesian& c, const Cartesian& d, const Cartesian& p) noexcept {
        auto vap = p - a;
        auto vbp = p - b;

        auto vab = b - a;
        auto vac = c - a;
        auto vad = d - a;

        auto vbc = c - b;
        auto vbd = d - b;

        auto va6 = scalarTripleProduct(vbp, vbd, vbc);
        auto vb6 = scalarTripleProduct(vap, vac, vad);
        auto vc6 = scalarTripleProduct(vap, vad, vab);
        auto vd6 = scalarTripleProduct(vap, vab, vac);
        auto v6 = 1 / scalarTripleProduct(vab, vac, vad);
        return Barycentric { va6 * v6, vb6 * v6, vc6 * v6, vd6 * v6 };
    }

}
