add_library(
    progressslider
    progressslider.cpp
)
target_link_libraries(
    progressslider
    strokelabel
    Qt5::Widgets
    Qt5::Core
)
list(APPEND PLAYER_OVERLAY_LIBS progressslider)

add_library(
    playermenu
    playermenu.cpp
    playermenu.ui
)
target_link_libraries(
    playermenu
    Qt5::Widgets
)
list(APPEND PLAYER_OVERLAY_LIBS playermenu)

add_library(
    subtitlewidget
    subtitlewidget.cpp
)
target_link_libraries(
    subtitlewidget
    strokelabel
    Qt5::Widgets
    globalmediator
    dictionary_db
    mpvadapter
)
list(APPEND PLAYER_OVERLAY_LIBS subtitlewidget)

add_library(
    playercontrols
    playercontrols.ui
    playercontrols.cpp
)
target_link_libraries(
    playercontrols
    Qt5::Widgets
    iconfactory
    subtitlewidget
    progressslider
)
list(APPEND PLAYER_OVERLAY_LIBS playercontrols)

if(OCR_SUPPORT)
    add_library(
        ocroverlay
        ocroverlay.cpp
    )
    target_link_libraries(
        ocroverlay
        ocrmodel
        Qt5::Widgets
        Qt5::Concurrent
    )
    list(APPEND PLAYER_OVERLAY_LIBS ocroverlay)
endif()

add_library(
    playeroverlay
    playeroverlay.cpp
)
target_link_libraries(
    playeroverlay
    ${PLAYER_OVERLAY_LIBS}
)