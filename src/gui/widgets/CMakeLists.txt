find_library(MPV_LIB mpv)

add_library(mpvwidget mpvwidget.cpp)
target_link_libraries(mpvwidget ${MPV_LIB} Qt5::Widgets)