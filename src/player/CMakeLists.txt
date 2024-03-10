add_library(track INTERFACE track.h)
target_compile_features(track INTERFACE cxx_std_17)
target_compile_options(track INTERFACE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(track INTERFACE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    track
    INTERFACE Qt6::Core
)

add_library(playeradapter STATIC playeradapter.h)
target_compile_features(playeradapter PUBLIC cxx_std_17)
target_compile_options(playeradapter PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(playeradapter PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    playeradapter
    PUBLIC Qt6::Core
    PUBLIC Qt6::Gui
    PUBLIC track
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
