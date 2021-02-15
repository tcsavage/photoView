#pragma once

#include <memory>
#include <vector>

#include <image/CoreTypes.hpp>
#include <image/Filters.hpp>
#include <image/ImageBuf.hpp>
#include <image/NDArray.hpp>
#include <image/luts/Lattice3D.hpp>
#include <image/opencl/Program.hpp>

namespace image {

    struct Look {
        std::vector<FilterSpec> filterSpecs;

        FilterSpec &addFilter(FilterSpec &&filter) noexcept;
    };

    struct Processor {
        std::shared_ptr<Look> look;

        luts::Lattice3D lattice { 32 };
        NDArray<F32> latticeImage;

        opencl::Program oclProgram;
        opencl::Kernel oclKernel;
        opencl::SamplerHandle oclSampler;

        void init() noexcept;
        void update() noexcept;
        void syncLattice() noexcept;
        void process(ImageBuf<F32> &in, ImageBuf<U8> &out) noexcept;

        explicit Processor() noexcept : look(nullptr) {}
        explicit Processor(std::shared_ptr<Look> look) noexcept : look(look) {}
    };

}
