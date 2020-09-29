find_library(MPV_LIB mpv)

add_library(sliderjumpstyle
    sliderjumpstyle.cpp)
target_link_libraries(sliderjumpstyle
    Qt5::Widgets)

add_library(mpvwidget
    mpvwidget.cpp)
target_link_libraries(mpvwidget
    ${MPV_LIB}
    Qt5::Widgets)

add_library(subtitlelabel
    subtitlelabel.cpp)
target_link_libraries(subtitlelabel
    Qt5::Widgets)

add_library(playercontrols
    playercontrols.ui
    playercontrols.cpp)
target_link_libraries(playercontrols
    Qt5::Widgets
    iconfactory
    sliderjumpstyle
    subtitlelabel)