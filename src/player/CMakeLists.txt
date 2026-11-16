add_library(
    mpvplayer
    mpvcontroller.cpp
    mpvcontroller.h
    mpvplayer.cpp
    mpvplayer.h
    mpvstate.cpp
    mpvstate.h
    mpvsubtitle.cpp
    mpvsubtitle.h
    mpvthumbnail.cpp
    mpvthumbnail.h
    mpvtrack.cpp
    mpvtrack.h
)
if(WIN32)
    target_sources(
        mpvplayer
        PRIVATE mpvd3d11renderer.cpp
        PRIVATE mpvd3d11renderer.h
    )
else()
    target_sources(
        mpvplayer
        PRIVATE mpvopenglframebackend.cpp
        PRIVATE mpvopenglframebackend.h
    )
    if(NOT APPLE)
        target_sources(
            mpvplayer
            PRIVATE mpvopenglrenderer.cpp
            PRIVATE mpvopenglrenderer.h
        )
    endif()
endif()
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
    )
    qt_add_shaders(
        mpvplayer mpv_shaders
        PREFIX "/player"
        BASE "shaders"
        GLSL "330"
        FILES shaders/mpv.vert shaders/mpv.frag
    )
endif()
target_compile_features(mpvplayer PRIVATE cxx_std_20)
target_include_directories(mpvplayer PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(mpvplayer PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    mpvplayer
    PRIVATE utils
    PRIVATE Qt6::GuiPrivate
    PUBLIC mpv::mpv
    PUBLIC Qt6::Core
    PUBLIC Qt6::Gui
    PUBLIC Qt6::OpenGL
    PUBLIC Qt6::Qml
    PUBLIC Qt6::Quick
)
