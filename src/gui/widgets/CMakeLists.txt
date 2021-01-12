find_library(MPV_LIB mpv)
find_library(MECAB_LIB mecab)

add_library(
    sliderjumpstyle
    sliderjumpstyle.cpp
)
target_link_libraries(
    sliderjumpstyle
    Qt5::Widgets
)

add_library(
    mpvwidget
    mpvwidget.cpp
    qthelper.h
)
target_link_libraries(
    mpvwidget
    ${MPV_LIB}
    directoryutils
    Qt5::Widgets
)

add_library(
    definitionwidget
    definitionwidget.cpp
    definitionwidget.ui
    definition.cpp
    definition.ui
)
target_link_libraries(
    definitionwidget
    Qt5::Widgets
    jmdict
)

add_library(
    subtitlewidget
    subtitlewidget.cpp
)
target_link_libraries(
    subtitlewidget
    Qt5::Widgets
    jmdict
    mecab
)

add_library(
    playercontrols
    playercontrols.ui
    playercontrols.cpp
)
target_link_libraries(
    playercontrols
    Qt5::Widgets
    iconfactory
    sliderjumpstyle
    subtitlewidget
)

add_library(
    ankisettings
    ankisettings.cpp
    ankisettings.ui
)

target_link_libraries(
    ankisettings
    Qt5::Widgets
    anki
)
