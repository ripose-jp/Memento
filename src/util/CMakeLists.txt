add_library(
    utils
    directoryutils.cpp
    directoryutils.h
    fileutils.cpp
    fileutils.h
    imageutils.cpp
    imageutils.h
    utils.h
)
target_compile_features(utils PUBLIC cxx_std_20)
target_compile_options(utils PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(utils PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    utils
    PUBLIC Qt6::Core
    PUBLIC Qt6::Gui
    PUBLIC Qt6::Qml
)
