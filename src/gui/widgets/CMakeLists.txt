add_subdirectory(common)
add_subdirectory(definition)
add_subdirectory(mpv)
add_subdirectory(overlay)
add_subdirectory(settings)

add_library(
    aboutwindow STATIC
    aboutwindow.cpp
    aboutwindow.h
)
target_compile_features(aboutwindow PUBLIC cxx_std_20)
target_compile_options(aboutwindow PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(aboutwindow PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    aboutwindow
    PRIVATE version
    PUBLIC Qt6::Widgets
)

add_library(
    searchwidget STATIC
    searchwidget.cpp
    searchwidget.h
)
target_compile_features(searchwidget PUBLIC cxx_std_20)
target_compile_options(searchwidget PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(searchwidget PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    searchwidget
    PRIVATE constants
    PRIVATE dictionary_db
    PRIVATE utils
    PUBLIC context
    PUBLIC definitionwidget
    PUBLIC Qt6::Widgets
)

add_library(
    subtitlelist STATIC
    subtitlelistwidget.cpp
    subtitlelistwidget.h
    subtitlelistwidget.ui
)
target_compile_features(subtitlelist PUBLIC cxx_std_20)
target_compile_options(subtitlelist PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(subtitlelist PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    subtitlelist
    PRIVATE anki
    PRIVATE constants
    PRIVATE definitionwidget
    PRIVATE iconfactory
    PRIVATE playeradapter
    PRIVATE subtitleparser
    PRIVATE utils
    PUBLIC context
    PUBLIC Qt6::Widgets
)
