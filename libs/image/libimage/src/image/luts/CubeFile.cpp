#include <image/luts/CubeFile.hpp>

#include <cmath>
#include <iostream>

namespace image::luts {

    namespace {
        std::istream &operator>>(std::istream &input, ColorRGB<F32> &color) {
            input >> color.r;
            input >> color.g;
            input >> color.b;
            return input;
        }

        std::ostream &operator<<(std::ostream &output, const ColorRGB<F32> &color) {
            output << color.r << " " << color.g << " " << color.b;
            return output;
        }
    }

    std::istream &operator>>(std::istream &stream, CubeFile &cube) {
        String field;

        while (!stream.eof()) {
            char first = static_cast<char>(stream.peek());
            if (first == '\n' || first == '\r' || first == '\t' || first == ' ') {
                stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            } else if (first == '#') {
                stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            } else if (!(first >= 'A' && first <= 'Z')) {
                break;
            }
            stream >> field;
            if (field == "TITLE") {
                stream.ignore(std::numeric_limits<std::streamsize>::max(), '"');
                char buf[250];
                stream.getline(buf, std::numeric_limits<std::streamsize>::max(), '"');
                cube.title = buf;
            } else if (field == "LUT_3D_SIZE") {
                stream >> cube.size;
            } else if (field == "DOMAIN_MIN") {
                stream >> cube.domainMin;
            } else if (field == "DOMAIN_MAX") {
                stream >> cube.domainMax;
            } else {
                stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }

        if (cube.size == 0) {
            return stream;
        }

        cube.table = NDArray<ColorRGB<F32>>(Shape{ cube.size, cube.size, cube.size });

        for (std::size_t i = 0; i < cube.table.size(); i++) {
            stream >> cube.table.at(i);
        }

        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const CubeFile &cube) {
        if (!cube.title.empty()) {
            stream << "TITLE " << cube.title << std::endl;
        }

        stream << "LUT_3D_SIZE " << cube.size << std::endl;

        stream << "DOMAIN_MIN " << cube.domainMin << std::endl;
        stream << "DOMAIN_MAX " << cube.domainMax << std::endl;

        for (const auto& color : cube.table) {
            stream << color << "\n";
        }

        return stream;
    }

    CubeFile::CubeFile(Lattice3D &lattice) noexcept
        : size(lattice.size)
        , domainMin(lattice.domainMin)
        , domainMax(lattice.domainMax)
        , table(lattice.table) {}

    Lattice3D CubeFile::lattice() const noexcept {
        Lattice3D out(size);
        out.domainMin = domainMin;
        out.domainMax = domainMax;
        out.table = table;
        return out;
    }

}
