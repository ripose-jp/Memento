add_library(
    globalmediator STATIC
    globalmediator.cpp
    globalmediator.h
)
target_compile_features(globalmediator PUBLIC cxx_std_17)
target_compile_options(globalmediator PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(globalmediator PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    globalmediator
    PUBLIC Qt5::Core
)

add_library(
    utils STATIC
    utils.cpp
    utils.h
)
target_compile_features(utils PUBLIC cxx_std_17)
target_compile_options(utils PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(utils PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    utils
    PRIVATE Qt5::Network
    PRIVATE version
    PUBLIC Qt5::Core
)

add_library(
    iconfactory STATIC
    iconfactory.cpp
    iconfactory.h
)
target_compile_features(iconfactory PUBLIC cxx_std_17)
target_compile_options(iconfactory PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(iconfactory PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    iconfactory
    PRIVATE Qt5::Core
    PUBLIC Qt5::Widgets
)

add_library(
    subtitleparser STATIC
    subtitleparser.cpp
    subtitleparser.h
)
target_compile_features(subtitleparser PUBLIC cxx_std_17)
target_compile_options(subtitleparser PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(subtitleparser PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    subtitleparser
    PUBLIC Qt5::Core
)
