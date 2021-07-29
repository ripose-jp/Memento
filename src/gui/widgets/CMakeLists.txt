add_subdirectory(common)

add_library(
    mpvwidget
    mpvwidget.cpp
)
set(
    MPV_WIDGET_LIBS
    ${MPV_LIB}
    utils
    Qt5::Widgets
)
if(UNIX AND NOT APPLE)
    set(
        MPV_WIDGET_LIBS
	    ${MPV_WIDGET_LIBS}
	    Qt5::DBus
    )
elseif(UNIX AND APPLE)
    set(
        MPV_WIDGET_LIBS
        ${MPV_WIDGET_LIBS}
        macos_power_handler
    )
endif()
target_link_libraries(
    mpvwidget
    ${MPV_WIDGET_LIBS}
)

add_library(
    aboutwindow
    aboutwindow.cpp
)
target_link_libraries(
    aboutwindow
    Qt5::Widgets
    Qt5::Core
)

add_library(
    subtitlewidget
    subtitlewidget.cpp
)
target_link_libraries(
    subtitlewidget
    Qt5::Widgets
    globalmediator
    dictionary_db
    mpvadapter
)

add_library(
    subtitlelist
    subtitlelistwidget.cpp
    subtitlelistwidget.ui
)
target_link_libraries(
    subtitlelist
    Qt5::Widgets
    mpvadapter
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
    progressslider
)

add_subdirectory(settings)
add_subdirectory(definition)