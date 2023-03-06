add_subdirectory(widgets)

add_library(
    mainwindow STATIC
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)
target_compile_features(mainwindow PUBLIC cxx_std_17)
target_compile_options(mainwindow PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(mainwindow PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    mainwindow
    PRIVATE "$<$<BOOL:${APPLE}>:cocoa_event_handler>"
    PRIVATE aboutwindow
    PRIVATE mpvadapter
    PRIVATE optionswindow
    PRIVATE playeroverlay
    PRIVATE searchwidget
    PUBLIC Qt5::Widgets
)
