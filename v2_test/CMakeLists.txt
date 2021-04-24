set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets CONFIG REQUIRED)

set(photoView_v2_test_sources
    resources.qrc
    src/app/main.cpp
    src/app/MainWindow.cpp
    src/app/NodeTree.cpp
    src/app/NodeTreeModel.cpp
    src/app/ProgramModel.cpp
    src/app/QtSupport.cpp
)

add_executable(photoView_v2_test ${photoView_v2_test_sources})

target_include_directories(photoView_v2_test PUBLIC src)

target_link_libraries(
    photoView_v2_test
    PRIVATE
    Qt5::Widgets
    image::libimage
)

target_compile_features(photoView_v2_test PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(photoView_v2_test PRIVATE /W4)
else()
    target_compile_options(photoView_v2_test PRIVATE -Wall -Wextra -pedantic -Werror)
    # if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT photoView_v2_test_DISABLE_ASAN)
    #     target_compile_options(photoView_v2_test PRIVATE -fsanitize=address)
    #     target_link_libraries(photoView_v2_test PRIVATE -fsanitize=address)
    # endif()
    if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_compile_options(libimage PRIVATE -fno-omit-frame-pointer)
    endif()
endif()

include(GNUInstallDirs)
install(TARGETS photoView_v2_test RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
