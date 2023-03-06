add_library(
    mpvwidget STATIC
    cursortimer.h
    mpvtimer.cpp
    mpvwidget.cpp
    mpvwidget.h
    osctimer.cpp
)
target_compile_features(mpvwidget PUBLIC cxx_std_17)
target_compile_options(mpvwidget PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(mpvwidget PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    mpvwidget
    PRIVATE "$<$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>:Qt5::DBus>"
    PRIVATE "$<$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>:Qt5::X11Extras>"
    PRIVATE "$<$<BOOL:${APPLE}>:-framework CoreFoundation>"
    PRIVATE "$<$<BOOL:${APPLE}>:-framework IOKit>"
    PRIVATE globalmediator
    PRIVATE mpv::mpv
    PUBLIC Qt5::Widgets
)
