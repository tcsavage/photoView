#include <image/Stopwatch.hpp>

#include <iostream>

namespace image {

    Stopwatch::Stopwatch(std::string name) : name(name), start(std::chrono::steady_clock::now()) {}

    Stopwatch::~Stopwatch() {
        auto end = std::chrono::steady_clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cerr << "[Stopwatch] " << name << " took " << dt.count() << " ms\n";
    }

}
