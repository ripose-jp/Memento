find_library(MPV_LIB mpv)

add_library(sliderjumpstyle
    sliderjumpstyle.cpp)
target_link_libraries(sliderjumpstyle
    Qt5::Widgets)

add_library(mpvwidget
    mpvwidget.cpp)
target_link_libraries(mpvwidget
    ${MPV_LIB}
    directoryutils
    Qt5::Widgets)

add_library(subtitlewidget
    subtitlewidget.cpp)
target_link_libraries(subtitlewidget
    Qt5::Widgets)

add_library(playercontrols
    playercontrols.ui
    playercontrols.cpp)
target_link_libraries(playercontrols
    Qt5::Widgets
    iconfactory
    sliderjumpstyle
    subtitlewidget)