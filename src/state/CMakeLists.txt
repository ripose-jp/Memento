add_library(
    context
    context.cpp
    context.h
)
target_compile_features(context PRIVATE cxx_std_20)
target_include_directories(context PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(context PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    context
    PUBLIC anki
    PUBLIC audioplayer
    PUBLIC dictionary
    PUBLIC fileopenhandler
    PUBLIC keytracker
    PUBLIC mpvplayer
    PUBLIC Qt6::Core
    PUBLIC settings
    PUBLIC subtitle
    PUBLIC utils
)
