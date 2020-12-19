cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(
    photo_luts
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "Apply LUTs to photographs"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

add_subdirectory(app)
