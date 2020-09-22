find_package(Qt5Widgets)
find_package(Qt5Gui)
find_package(Qt5Core)
find_library(MPV_LIB mpv)

add_library(mpvwidget mpvwidget.cpp)
target_link_libraries(mpvwidget Qt5::Gui Qt5::Core "${MPV_LIB}")

add_library(mainwindow mainwindow.cpp)
target_link_libraries(mainwindow Qt5::Widgets mpvwidget)