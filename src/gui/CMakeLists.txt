add_subdirectory(widgets)

add_library(
    mpvadapter
    mpvadapter.cpp
    playeradapter.h
)
target_link_libraries(
    mpvadapter 
    mpvwidget
    Qt5::Gui
    Qt5::Core
)

set(
    GUI_SRCS
    mainwindow.cpp
    mainwindow.ui
)

set(
    GUI_LIBS
    Qt5::Gui
    Qt5::Core
    mpvwidget
    mpvadapter
    playercontrols
    definitionwidget
    ankisettings
    anki
    jmdict_builder
)

add_library(mainwindow ${GUI_SRCS})
target_link_libraries(mainwindow ${GUI_LIBS})