add_library(
    screensaver
    screensaver.cpp
    screensaver.h
)
target_compile_features(screensaver PRIVATE cxx_std_20)
target_include_directories(screensaver PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(screensaver PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    screensaver
    PUBLIC Qt6::Core
)
if(UNIX AND NOT APPLE)
    target_link_libraries(
        screensaver
        PRIVATE Qt6::DBus
    )
endif()
