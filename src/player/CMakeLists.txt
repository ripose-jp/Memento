add_library(playeradapter STATIC playeradapter.h)
target_compile_features(playeradapter PUBLIC cxx_std_17)
target_compile_options(playeradapter PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(playeradapter PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    playeradapter
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
)

add_library(
    mpvadapter STATIC
    mpvadapter.cpp
    mpvadapter.h
)
target_compile_features(mpvadapter PUBLIC cxx_std_17)
target_compile_options(mpvadapter PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(mpvadapter PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    mpvadapter
    PRIVATE globalmediator
    PRIVATE mpv::mpv
    PRIVATE mpvwidget
    PUBLIC playeradapter
)
