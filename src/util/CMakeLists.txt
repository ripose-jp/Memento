add_library(
    directoryutils
    directoryutils.cpp
)
target_link_libraries(
    directoryutils
    Qt5::Core
)

add_library(
    fileutils
    fileutils.cpp
)
target_link_libraries(
    fileutils
    Qt5::Core
)

add_library(
    graphicutils
    graphicutils.cpp
)
target_link_libraries(
    graphicutils
    Qt5::Core
)

add_library(
    globalmediator
    globalmediator.cpp
)
target_link_libraries(
    globalmediator
    Qt5::Core
)

add_library(
    iconfactory 
    iconfactory.h
    iconfactory.cpp
)
target_link_libraries(
    iconfactory
    Qt5::Gui
    Qt5::Widgets
)

if (APPLE)
    add_library(macos_power_handler macospowereventhandler.cpp)
    target_link_libraries(macos_power_handler "-framework IOKit -framework CoreFoundation")
endif()