add_subdirectory(widgets)

set(GUI_SRCS
    mainwindow.cpp
    mainwindow.ui)

set(GUI_LIBS
    Qt5::Gui
    Qt5::Core
    mpvwidget
    playercontrols)

add_library(mainwindow ${GUI_SRCS})
target_link_libraries(mainwindow ${GUI_LIBS})