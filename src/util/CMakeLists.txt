add_library(directoryutils directoryutils.cpp)
target_link_libraries(directoryutils Qt::Core)

add_library(iconfactory 
    iconfactory.h
    iconfactory.cpp)
target_link_libraries(iconfactory
    Qt5::Gui
    Qt5::Widgets)