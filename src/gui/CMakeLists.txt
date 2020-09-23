find_library(MPV_LIB mpv)

set(GUI_SRCS
    mpvwidget.cpp
    mainwindow.cpp
    mainwindow.ui)

set(GUI_LIBS
    Qt5::Gui
    Qt5::Core
    Qt5::Widgets
    ${MPV_LIB})

add_library(mainwindow ${GUI_SRCS})
target_link_libraries(mainwindow ${GUI_LIBS})