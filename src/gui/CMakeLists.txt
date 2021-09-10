add_library(
    mpvadapter
    mpvadapter.cpp
    playeradapter.h
)
target_link_libraries(
    mpvadapter
    globalmediator
    mpvwidget
    Qt5::Gui
    Qt5::Core
)

add_subdirectory(widgets)

set(
    GUI_SRCS
    mainwindow.cpp
    mainwindow.ui
)

set(
    GUI_LIBS
    Qt5::Gui
    Qt5::Core
    aboutwindow
    anki
    definitionwidget
    dictionary_db
    globalmediator
    mpvadapter
    mpvwidget
    optionswindow
    playeroverlay
    sliderjumpstyle
    subtitlelist
)

if (APPLE)
    set(GUI_LIBS ${GUI_LIBS} cocoa_event_handler)
endif()

add_library(mainwindow ${GUI_SRCS})
target_link_libraries(mainwindow ${GUI_LIBS})