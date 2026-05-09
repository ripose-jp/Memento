add_library(
    dictionary_data
    data.h
    dictionaryinfo.cpp
    dictionaryinfo.h
    expression.cpp
    expression.h
    frequency.cpp
    frequency.h
    kanji.cpp
    kanji.h
    kanjidefinition.cpp
    kanjidefinition.h
    pitch.cpp
    pitch.h
    tag.cpp
    tag.h
    term.cpp
    term.h
    termdefinition.cpp
    termdefinition.h
)
target_compile_features(dictionary_data PRIVATE cxx_std_20)
target_compile_options(dictionary_data PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(dictionary_data PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    dictionary_data
    PUBLIC Qt6::Core
    PUBLIC Qt6::Qml
)
