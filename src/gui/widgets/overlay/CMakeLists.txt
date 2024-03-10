add_library(
    playeroverlay STATIC
    "$<$<BOOL:${OCR_SUPPORT}>:ocroverlay.cpp>"
    "$<$<BOOL:${OCR_SUPPORT}>:ocroverlay.h>"
    playercontrols.cpp
    playercontrols.h
    playercontrols.ui
    playermenu.cpp
    playermenu.h
    playermenu.ui
    playeroverlay.cpp
    playeroverlay.h
    progressslider.cpp
    progressslider.h
    subtitlewidget.cpp
    subtitlewidget.h
)
target_compile_features(playeroverlay PUBLIC cxx_std_17)
target_compile_options(playeroverlay PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(playeroverlay PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    playeroverlay
    PRIVATE "$<$<BOOL:${OCR_SUPPORT}>:ocrmodel>"
    PRIVATE dictionary_db
    PRIVATE hittestwidget
    PRIVATE iconfactory
    PRIVATE Qt6::Concurrent
    PRIVATE strokelabel
    PUBLIC playeradapter
    PUBLIC Qt6::OpenGLWidgets
    PUBLIC Qt6::Widgets
)
