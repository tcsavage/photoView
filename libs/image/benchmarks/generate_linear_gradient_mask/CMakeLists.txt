find_package(benchmark REQUIRED)

add_executable(libimage_benchmark_generate_linear_gradient_mask main.cpp)
target_link_libraries(libimage_benchmark_generate_linear_gradient_mask PUBLIC image::libimage benchmark::benchmark)

target_compile_features(libimage_benchmark_generate_linear_gradient_mask PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(libimage_benchmark_generate_linear_gradient_mask PRIVATE /W4 /WX)
else()
    target_compile_options(libimage_benchmark_generate_linear_gradient_mask PRIVATE -Wall -Wextra -pedantic -Werror -march=native)
endif()

if(MSVC)
    target_compile_options(libimage_benchmark_generate_linear_gradient_mask PRIVATE /arch:AVX2)
else()
    target_compile_options(libimage_benchmark_generate_linear_gradient_mask PRIVATE -mavx2)
endif()

include(GNUInstallDirs)
install(TARGETS libimage_benchmark_generate_linear_gradient_mask RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
