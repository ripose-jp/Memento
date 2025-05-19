add_subdirectory(widgets)

add_library(
    mainwindow STATIC
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)
target_compile_features(mainwindow PUBLIC cxx_std_20)
target_compile_options(mainwindow PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(mainwindow PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    mainwindow
    PRIVATE "$<$<BOOL:${APPLE}>:cocoa_event_handler>"
    PRIVATE "$<$<BOOL:${WIN32}>:Qt6::GuiPrivate>"
    PRIVATE aboutwindow
    PRIVATE constants
    PRIVATE mpvadapter
    PRIVATE optionswindow
    PRIVATE playeroverlay
    PRIVATE searchwidget
    PUBLIC anki
    PUBLIC audioplayer
    PUBLIC context
    PUBLIC dictionary_db
    PUBLIC Qt6::Widgets
)
