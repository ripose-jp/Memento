add_library(
    context STATIC
    context.cpp
    context.h
)
target_compile_features(context PUBLIC cxx_std_20)
target_compile_options(context PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(context PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    context
    PRIVATE anki
    PRIVATE audioplayer
    PRIVATE dictionary_db
    PRIVATE playeradapter
    PRIVATE subtitlelist
    PUBLIC Qt6::Core
    PUBLIC track
)
