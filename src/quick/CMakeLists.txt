add_library(
    clipboard
    clipboard.cpp
    clipboard.h
)
target_compile_features(clipboard PRIVATE cxx_std_20)
target_include_directories(clipboard PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(clipboard PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    clipboard
    PRIVATE Qt6::Gui
    PUBLIC Qt6::Core
)

add_library(
    coloredsvgprovider
    coloredsvgprovider.cpp
    coloredsvgprovider.h
)
target_compile_features(coloredsvgprovider PRIVATE cxx_std_20)
target_include_directories(coloredsvgprovider PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(coloredsvgprovider PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    coloredsvgprovider
    PRIVATE Qt6::Gui
    PRIVATE Qt6::Svg
    PUBLIC Qt6::Quick
)

add_library(
    fileopenhandler
    fileopenhandler.cpp
    fileopenhandler.h
)
target_compile_features(fileopenhandler PRIVATE cxx_std_20)
target_include_directories(fileopenhandler PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(fileopenhandler PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    fileopenhandler
    PRIVATE Qt6::Gui
    PUBLIC Qt6::Core
)

add_library(
    features
    features.cpp
    features.h
)
target_compile_features(features PRIVATE cxx_std_20)
target_include_directories(features PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(features PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    features
    PRIVATE version
    PUBLIC Qt6::Core
)

add_library(
    keytracker
    keytracker.cpp
    keytracker.h
)
target_compile_features(keytracker PRIVATE cxx_std_20)
target_include_directories(keytracker PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(keytracker PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    keytracker
    PRIVATE Qt6::Gui
    PUBLIC Qt6::Core
)


add_library(
    quickpaths
    paths.cpp
    paths.h
)
target_compile_features(quickpaths PRIVATE cxx_std_20)
target_include_directories(quickpaths PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(quickpaths PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    quickpaths
    PUBLIC Qt6::Core
    PUBLIC utils
)

add_library(
    historymanager
    historymanager.cpp
    historymanager.h
)
target_compile_features(historymanager PRIVATE cxx_std_20)
target_include_directories(historymanager PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(historymanager PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    historymanager
    PUBLIC Qt6::Core
    PUBLIC utils
)
