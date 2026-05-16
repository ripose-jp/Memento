add_library(
    mpvplayer
    mpvcontroller.cpp
    mpvcontroller.h
    mpvframebackend.cpp
    mpvframebackend.h
    mpvplayer.cpp
    mpvplayer.h
    mpvrenderer.cpp
    mpvrenderer.h
    mpvstate.cpp
    mpvstate.h
    mpvsubtitle.cpp
    mpvsubtitle.h
    mpvtrack.cpp
    mpvtrack.h
)
if(APPLE)
    target_sources(
        mpvplayer
        PRIVATE mpvmetalrenderer.h
        PRIVATE mpvmetalrenderer.mm
    )
    target_link_libraries(
        mpvplayer
        PRIVATE "-framework CoreVideo"
        PRIVATE "-framework IOSurface"
        PRIVATE "-framework Metal"
        PRIVATE "-framework OpenGL"
        PRIVATE Qt6::QuickPrivate
    )
endif()
target_compile_features(mpvplayer PRIVATE cxx_std_20)
target_include_directories(mpvplayer PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(mpvplayer PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    mpvplayer
    PRIVATE Qt6::Gui
    PRIVATE utils
    PUBLIC mpv::mpv
    PUBLIC Qt6::Core
    PUBLIC Qt6::OpenGL
    PUBLIC Qt6::Qml
    PUBLIC Qt6::Quick
)
