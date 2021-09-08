add_library(
    globalmediator
    globalmediator.cpp
)
target_link_libraries(
    globalmediator
    Qt5::Core
)

add_library(
    utils
    utils.cpp
)
target_link_libraries(
    utils
    globalmediator
    Qt5::Core
    Qt5::Network
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
    add_library(
        cocoa_event_handler
        cocoaeventhandler.mm
    )
    target_link_libraries(
        cocoa_event_handler
        "-framework cocoa"
    )

    add_library(
        macos_power_handler
        macospowereventhandler.cpp
    )
    target_link_libraries(
        macos_power_handler
        "-framework IOKit -framework CoreFoundation"
    )
endif()