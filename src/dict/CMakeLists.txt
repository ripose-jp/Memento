add_library(
    yomidbbuilder STATIC
    yomidbbuilder.c
    yomidbbuilder.h
)
target_compile_features(yomidbbuilder PUBLIC c_std_99)
target_compile_options(yomidbbuilder PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(yomidbbuilder PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    yomidbbuilder
    PRIVATE "$<$<BOOL:${WIN32}>:-lregex>"
    PRIVATE JsonC::JsonC
    PRIVATE libzip::libzip
    PRIVATE SQLite::SQLite3
)

add_library(
    dictionary_db STATIC
    databasemanager.cpp
    databasemanager.h
    dictionary.cpp
    dictionary.h
    expression.h
)
target_compile_features(dictionary_db PUBLIC cxx_std_17)
target_compile_options(dictionary_db PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(dictionary_db PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    dictionary_db
    PRIVATE MeCab::MeCab
    PRIVATE Qt5::Widgets
    PRIVATE SQLite::SQLite3
    PRIVATE yomidbbuilder
    PUBLIC Qt5::Core
)
