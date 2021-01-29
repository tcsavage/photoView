# OpenImageIO
find_package(OpenImageIO REQUIRED)

# OpenCL
find_package(OpenCL REQUIRED)

# resources
include(../../../external/cmrc/CMakeRC)
cmrc_add_resource_library(
    libimage-resources
    ALIAS image::rc
    NAMESPACE image::rc
    kernels/apply3DLut_F32_U8.cl
)

# libimage
add_library(libimage
    src/image/Exception.cpp
    src/image/ImageProcessor.cpp
    src/image/Type.cpp
    src/image/luts/Barycentric.cpp
    src/image/luts/CubeFile.cpp
    src/image/luts/Lattice3D.cpp
    src/image/luts/SimpleCube.cpp
    src/image/luts/TetrahedralInterpolator.cpp
    src/image/memory/Allocator.cpp
    src/image/opencl/BufferDevice.cpp
    src/image/opencl/Context.cpp
    src/image/opencl/Manager.cpp
    src/image/opencl/Program.cpp
)
add_library(image::libimage ALIAS libimage)

target_include_directories(libimage PUBLIC include)

target_link_libraries(libimage
    PUBLIC
    glm::glm
    OpenImageIO::OpenImageIO
    OpenCL::OpenCL
    PRIVATE
    image::rc
)

target_compile_features(libimage PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(libimage PRIVATE /W4 /WX)
else()
    target_compile_options(libimage PRIVATE -Wall -Wextra -pedantic -Werror -march=native)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT IMAGE_DISABLE_ASAN)
        target_compile_options(libimage PRIVATE -fsanitize=address)
        target_link_libraries(libimage PRIVATE -fsanitize=address)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_compile_options(libimage PRIVATE -fno-omit-frame-pointer)
    endif()
endif()
# enable_clang_tidy(libimage)

include(GNUInstallDirs)
install(TARGETS libimage RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
