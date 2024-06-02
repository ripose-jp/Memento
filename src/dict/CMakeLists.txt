add_library(searchquery INTERFACE searchquery.h)
target_compile_features(searchquery INTERFACE cxx_std_17)
target_compile_options(searchquery INTERFACE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(searchquery INTERFACE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(searchquery INTERFACE Qt6::Core)

add_library(querygenerator INTERFACE querygenerator.h)
target_compile_features(querygenerator INTERFACE cxx_std_17)
target_compile_options(querygenerator INTERFACE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(querygenerator INTERFACE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    querygenerator
    INTERFACE Qt6::Core
    INTERFACE searchquery
)

add_library(
    exactquerygenerator STATIC
    exactquerygenerator.cpp
    exactquerygenerator.h
)
target_compile_features(exactquerygenerator PRIVATE cxx_std_17)
target_compile_options(exactquerygenerator PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(exactquerygenerator PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    exactquerygenerator
    PUBLIC querygenerator
)

if(MECAB_SUPPORT)
    add_library(
        mecabquerygenerator STATIC
        mecabquerygenerator.cpp
        mecabquerygenerator.h
    )
    target_compile_features(mecabquerygenerator PRIVATE cxx_std_17)
    target_compile_options(
        mecabquerygenerator PRIVATE ${MEMENTO_COMPILER_FLAGS}
    )
    target_include_directories(
        mecabquerygenerator PRIVATE ${MEMENTO_INCLUDE_DIRS}
    )
    target_link_libraries(
        mecabquerygenerator
        PRIVATE utils
        PUBLIC MeCab::MeCab
        PUBLIC querygenerator
    )
endif()

add_library(
    yomidbbuilder STATIC
    yomidbbuilder.c
    yomidbbuilder.h
)
target_compile_features(
    yomidbbuilder
    PUBLIC c_std_99
)
target_compile_options(
    yomidbbuilder
    PRIVATE ${MEMENTO_COMPILER_FLAGS}
    PRIVATE "$<$<STREQUAL:${CMAKE_C_COMPILER_ID},AppleClang>:-Wno-nullability-extension>"
)
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
    deconjugator.cpp
    deconjugator.h
    deconjugationquerygenerator.cpp
    deconjugationquerygenerator.h
)
target_compile_features(dictionary_db PUBLIC cxx_std_17)
target_compile_options(dictionary_db PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(dictionary_db PRIVATE ${MEMENTO_INCLUDE_DIRS})
set(
    DICTIONARY_DB_GENERATOR_LIBS
    exactquerygenerator
)
if(MECAB_SUPPORT)
    list(APPEND DICTIONARY_DB_GENERATOR_LIBS mecabquerygenerator)
endif()
target_link_libraries(
    dictionary_db
    PRIVATE ${DICTIONARY_DB_GENERATOR_LIBS}
    PRIVATE Qt6::Widgets
    PRIVATE querygenerator
    PRIVATE SQLite::SQLite3
    PRIVATE yomidbbuilder
    PUBLIC Qt6::Core
)
unset(DICTIONARY_DB_GENERATOR_LIBS)
