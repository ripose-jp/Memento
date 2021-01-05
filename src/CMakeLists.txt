add_subdirectory(util)
add_subdirectory(dict)
add_subdirectory(anki)
add_subdirectory(gui)

add_executable(memento 
               main.cpp 
               ../res/resources.qrc
               ../config/input.conf
               ../config/dict/JMDict)
target_link_libraries(memento mainwindow)