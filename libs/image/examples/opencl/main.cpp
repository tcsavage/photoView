/**
 * @brief Simple example for testing OpenCL features.
 */
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>

#include <image/NDArray.hpp>
#include <image/opencl/Context.hpp>
#include <image/opencl/Manager.hpp>
#include <image/opencl/Program.hpp>

int main(int, const char*[]) {
    image::opencl::Manager manager; // Singleton. Access with Manager::the()

    // Generate data.
    image::NDArray<image::U32> arr1 { 256 };
    image::NDArray<image::U32> arr2 { 256 };
    std::generate(std::begin(arr1), std::end(arr1), [n = 0]() mutable { return n++; });
    std::fill(std::begin(arr2), std::end(arr2), 0);

    // Set-up device.
    arr1.buffer()->device = image::opencl::Manager::the()->bufferDevice;
    arr1.buffer()->deviceMalloc();
    arr1.buffer()->copyHostToDevice();
    arr2.buffer()->device = image::opencl::Manager::the()->bufferDevice;
    arr2.buffer()->deviceMalloc();

    // load kernel source.
    std::ifstream ifs;
    ifs.open("libs/image/examples/opencl/copyKernel.cl");
    std::stringstream ss;
    ss << ifs.rdbuf();
    auto src = ss.str();

    // Create and build program.
    auto maybeProg = image::opencl::Program::fromSource(image::opencl::Manager::the()->context, src);
    if (maybeProg.hasError()) {
        std::cerr << maybeProg.error() << "\n";
    }
    auto buildResult = maybeProg.value().build();
    if (buildResult.hasError()) {
        std::cerr << buildResult.error() << "\n";
    }

    // Configure and run kernel...

    auto maybeKernel = maybeProg->getKernel("copy");
    if (maybeKernel.hasError()) {
        std::cerr << maybeKernel.error() << "\n";
    }

    auto setArgsResult = maybeKernel->setArgs(arr1, arr2);
    if (setArgsResult.hasError()) {
        std::cerr << setArgsResult.error().error << "\n";
    }

    maybeKernel->run(image::opencl::Manager::the()->queue.getHandle(), image::Shape { 256 });

    // Read data back and display.
    arr2.buffer()->copyDeviceToHost();
    std::cerr << "@55:  " << arr1.at(55)  << ", " << arr2.at(55)  << "\n";
    std::cerr << "@128: " << arr1.at(128) << ", " << arr2.at(128) << "\n";

    return 0;
}
