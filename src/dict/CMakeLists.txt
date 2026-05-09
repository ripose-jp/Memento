add_subdirectory(data)

add_library(searchquery INTERFACE searchquery.h)
target_compile_features(searchquery INTERFACE cxx_std_20)
target_compile_options(searchquery INTERFACE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(searchquery INTERFACE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(searchquery INTERFACE Qt6::Core)

add_library(querygenerator INTERFACE querygenerator.h)
target_compile_features(querygenerator INTERFACE cxx_std_20)
target_compile_options(querygenerator INTERFACE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(querygenerator INTERFACE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    querygenerator
    INTERFACE Qt6::Core
    INTERFACE searchquery
)

if(MEMENTO_MECAB_SUPPORT)
    add_library(
        mecabquerygenerator
        mecabquerygenerator.cpp
        mecabquerygenerator.h
    )
    target_compile_features(mecabquerygenerator PRIVATE cxx_std_20)
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
    yomidbbuilder
    yomidbbuilder.c
    yomidbbuilder.h
)
target_compile_features(yomidbbuilder PRIVATE c_std_99)
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
    PRIVATE SQLite3::SQLite3
)

add_library(
    dictionary
    databasemanager.cpp
    databasemanager.h
    deconjugationquerygenerator.cpp
    deconjugationquerygenerator.h
    deconjugator.cpp
    deconjugator.h
    dictionary.cpp
    dictionary.h
    dictionarycontroller.cpp
    dictionarycontroller.h
    dictionaryinfomodel.cpp
    dictionaryinfomodel.h
    dictionarysearch.cpp
    dictionarysearch.h
    exactquerygenerator.cpp
    exactquerygenerator.h
)
target_compile_features(dictionary PUBLIC cxx_std_20)
target_compile_options(dictionary PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(dictionary PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    dictionary
    PRIVATE "$<$<BOOL:${MEMENTO_MECAB_SUPPORT}>:mecabquerygenerator>"
    PRIVATE Qt6::Concurrent
    PRIVATE querygenerator
    PRIVATE SQLite3::SQLite3
    PRIVATE utils
    PRIVATE yomidbbuilder
    PUBLIC "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Coro>"
    PUBLIC "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Coro>"
    PUBLIC dictionary_data
    PUBLIC Qt6::Core
    PUBLIC Qt6::Qml
    PUBLIC settings
)
