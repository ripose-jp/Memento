add_subdirectory(util)
add_subdirectory(gui)

add_executable(Memento main.cpp)
target_link_libraries(Memento mainwindow kiten)