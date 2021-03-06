set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Widgets CONFIG REQUIRED)

set(photoView_sources
    resources.qrc
    src/app/main.cpp
    src/app/OpenFileState.cpp
    src/app/PhotoWindow.cpp
    src/app/canvas/CanvasControl.cpp
    src/app/canvas/CanvasItems.cpp
    src/app/canvas/CanvasScene.cpp
    src/app/canvas/CanvasView.cpp
    src/app/canvas/LinearGradientControl.cpp
    src/app/canvas/MaskOverlayControl.cpp
    src/app/composition/CompositionManager.cpp
    src/app/composition/CompositionModel.cpp
    src/app/composition/CompositionOutline.cpp
    src/app/dialogs/HaldImageDialog.cpp
    src/app/filters/FilterDialog.cpp
    src/app/filters/FilterManager.cpp
    src/app/filters/FilterWidget.cpp
    src/app/masks/MaskManager.cpp
    src/app/widgets/FileChooser.cpp
    src/app/widgets/ProcessingIndicator.cpp
)

add_executable(photoView ${photoView_sources})

target_include_directories(photoView PUBLIC src)

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
