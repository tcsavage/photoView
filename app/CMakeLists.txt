set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets CONFIG REQUIRED)

set(photoView_sources
    resources.qrc
    src/main.cpp
    src/CompositionManager.cpp
    src/HaldImageDialog.cpp
    src/ImageView.cpp
    src/OpenFileState.cpp
    src/PhotoWindow.cpp
    src/ProcessingIndicator.cpp
    src/filters/FilterDialog.cpp
    src/filters/FilterManager.cpp
    src/filters/FilterWidget.cpp
    src/filters/LookFilters.cpp
    src/filters/LookFiltersModel.cpp
    src/widgets/FileChooser.cpp
)

add_executable(photoView ${photoView_sources})

target_link_libraries(
    photoView
    PRIVATE
    Qt5::Widgets
    image::libimage
)

target_compile_features(photoView PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(photoView PRIVATE /W4)
else()
    target_compile_options(photoView PRIVATE -Wall -Wextra -pedantic -Werror)
    # if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT photoView_DISABLE_ASAN)
    #     target_compile_options(photoView PRIVATE -fsanitize=address)
    #     target_link_libraries(photoView PRIVATE -fsanitize=address)
    # endif()
    if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_compile_options(libimage PRIVATE -fno-omit-frame-pointer)
    endif()
endif()

include(GNUInstallDirs)
install(TARGETS photoView RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
