#pragma once

#include <chrono>

#include <image/CoreTypes.hpp>
#include <image/Util.hpp>

namespace image {

    class Stopwatch {
    public:
        Stopwatch(String name);
        ~Stopwatch();
    private:
        String name;
        std::chrono::time_point<std::chrono::steady_clock> start;
    };

}

#define STOPWATCH(name) RAII(Stopwatch, name)
