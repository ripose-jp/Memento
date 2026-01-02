add_library(
    utils STATIC
    utils.cpp
    utils.h
)
target_compile_features(utils PUBLIC cxx_std_20)
target_compile_options(utils PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(utils PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    utils
    PRIVATE Qt6::Network
    PRIVATE version
    PUBLIC "$<$<BOOL:${SYSTEM_QCORO}>:QCoro::Network>"
    PUBLIC "$<$<NOT:$<BOOL:${SYSTEM_QCORO}>>:QCoro6Network>"
    PUBLIC constants
    PUBLIC context
    PUBLIC Qt6::Core
    PUBLIC Qt6::Gui
)

add_library(constants INTERFACE constants.h)
target_compile_features(constants INTERFACE cxx_std_20)
target_compile_options(constants INTERFACE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(constants INTERFACE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(constants
    INTERFACE Qt6::Core
    INTERFACE utils
)

add_library(
    iconfactory STATIC
    iconfactory.cpp
    iconfactory.h
)
target_compile_features(iconfactory PUBLIC cxx_std_20)
target_compile_options(iconfactory PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(iconfactory PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    iconfactory
    PRIVATE Qt6::Core
    PUBLIC Qt6::Widgets
)

add_library(
    subtitleparser STATIC
    subtitleparser.cpp
    subtitleparser.h
)
target_compile_features(subtitleparser PUBLIC cxx_std_20)
target_compile_options(subtitleparser PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(subtitleparser PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    subtitleparser
    PUBLIC Qt6::Core
)
