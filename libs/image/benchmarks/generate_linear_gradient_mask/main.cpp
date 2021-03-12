#include <fstream>
#include <optional>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <image/CoreTypes.hpp>
#include <image/Mask.hpp>

using namespace image;

class GenerateFixture : public benchmark::Fixture {
public:
    Mask mask { 6000, 4000 };
    LinearGradientMaskSpec maskSpec;

    void SetUp(const benchmark::State &) {}

    void TearDown(const benchmark::State &) {}
};

struct LinearGradientProjection {
    glm::vec2 from;
    glm::vec2 to;

    F32 a;
    F32 b;

    F32 c1;
    F32 c2;

    constexpr F32 project(const glm::vec2 &point) const noexcept { return a * point.x + b * point.y; }

    constexpr F32 map(const glm::vec2 &point) const noexcept {
        auto proj = project(point);
        if (proj <= c1) { return 0.0f; }
        if (proj >= c2) { return 1.0f; }
        return (proj - c1) / (c2 - c1);
    }

    constexpr LinearGradientProjection(glm::vec2 from, glm::vec2 to)
        : from(from)
        , to(to)
        , a(to.x - from.x)
        , b(to.y - from.y)
        , c1(project(from))
        , c2(project(to)) {
        assert(c2 >= c1);
    }
};

void generate_sequential(LinearGradientMaskSpec &spec, Mask &mask) noexcept {
    LinearGradientProjection proj { spec.from, spec.to };
    glm::vec2 size { static_cast<F32>(mask.width()), static_cast<F32>(mask.height()) };
    for (memory::Size y = 0; y < mask.height(); ++y) {
        for (memory::Size x = 0; x < mask.width(); ++x) {
            glm::vec2 pos = glm::vec2 { static_cast<F32>(x), static_cast<F32>(y) } / size;
            auto value = proj.map(pos);
            mask.pixelArray.at(x, y) = value;
        }
    }
}

BENCHMARK_DEFINE_F(GenerateFixture, sequential)(benchmark::State &state) {
    for (auto _ : state) {
        generate_sequential(maskSpec, mask);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(GenerateFixture, sequential)->Unit(benchmark::kMillisecond);

void generate_omp_par_par(LinearGradientMaskSpec &spec, Mask &mask) noexcept {
    LinearGradientProjection proj { spec.from, spec.to };
    glm::vec2 size { static_cast<F32>(mask.width()), static_cast<F32>(mask.height()) };
    #pragma omp parallel for
    for (memory::Size y = 0; y < mask.height(); ++y) {
        #pragma omp parallel for
        for (memory::Size x = 0; x < mask.width(); ++x) {
            glm::vec2 pos = glm::vec2 { static_cast<F32>(x), static_cast<F32>(y) } / size;
            auto value = proj.map(pos);
            mask.pixelArray.at(x, y) = value;
        }
    }
}

BENCHMARK_DEFINE_F(GenerateFixture, openmp_par_par)(benchmark::State &state) {
    for (auto _ : state) {
        generate_omp_par_par(maskSpec, mask);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(GenerateFixture, openmp_par_par)->Unit(benchmark::kMillisecond);

void generate_omp_par_simd(LinearGradientMaskSpec &spec, Mask &mask) noexcept {
    LinearGradientProjection proj { spec.from, spec.to };
    glm::vec2 size { static_cast<F32>(mask.width()), static_cast<F32>(mask.height()) };
    #pragma omp parallel for
    for (memory::Size y = 0; y < mask.height(); ++y) {
        #pragma omp simd
        for (memory::Size x = 0; x < mask.width(); ++x) {
            glm::vec2 pos = glm::vec2 { static_cast<F32>(x), static_cast<F32>(y) } / size;
            auto value = proj.map(pos);
            mask.pixelArray.at(x, y) = value;
        }
    }
}

BENCHMARK_DEFINE_F(GenerateFixture, openmp_par_simd)(benchmark::State &state) {
    for (auto _ : state) {
        generate_omp_par_simd(maskSpec, mask);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(GenerateFixture, openmp_par_simd)->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();
