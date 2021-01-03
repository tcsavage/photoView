cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(
    image
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "Image manipulation library"
)

set(CMAKE_FOLDER "photoView/image")

option(IMAGE_BUILD_TESTS "Build tests targets for Image" BUILD_TESTING)
option(IMAGE_DISABLE_ASAN "Disable ASAN even in debug" OFF)
option(IMAGE_BUILD_EXAMPLES "Build example targets for image" YES)
option(IMAGE_BUILD_BENCHMARKS "Build benchmark targets for image" YES)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# include(ClangTidy)

add_subdirectory(libimage)

add_subdirectory(examples)
