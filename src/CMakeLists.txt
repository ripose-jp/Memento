add_subdirectory(util)
add_subdirectory(dict)
add_subdirectory(gui)

add_executable(Memento main.cpp ../res/resources.qrc)
target_link_libraries(Memento mainwindow)