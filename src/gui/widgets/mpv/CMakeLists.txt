add_library(
    mpvwidget STATIC
    cursortimer.h
    mpvtimer.cpp
    mpvwidget.cpp
    mpvwidget.h
    osctimer.cpp
)
target_compile_features(mpvwidget PUBLIC cxx_std_20)
target_compile_options(mpvwidget PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(mpvwidget PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    mpvwidget
    PRIVATE "$<$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>:Qt6::DBus>"
    PRIVATE "$<$<BOOL:${APPLE}>:-framework CoreFoundation>"
    PRIVATE "$<$<BOOL:${APPLE}>:-framework IOKit>"
    PRIVATE constants
    PRIVATE mpv::mpv
    PUBLIC context
    PUBLIC Qt6::OpenGLWidgets
    PUBLIC Qt6::Widgets
    PUBLIC utils
)
