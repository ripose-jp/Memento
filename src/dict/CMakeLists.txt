set(
    DICT_SRCS
    sqlite.h
    entry.h
    jmdict.cpp
    dictionary.cpp
)

set(
    BUILDER_SRCS
    builder/dictionarybuilder.cpp
    builder/xmlparser.h
    builder/kana2romaji.cpp
)

add_library(jmdict ${DICT_SRCS})
target_link_libraries(
    jmdict
    mecab
    ${SQLITE}
    Qt::Core
)

add_library(jmdict_builder ${BUILDER_SRCS})
target_link_libraries(
    jmdict_builder
    ${SQLITE}
    ${EXPAT}
)

add_library(
    dictionary_db
    dictionary.cpp
    databasemanager.cpp
    yomidbbuilder.c
    expression.h
)
target_link_libraries(
    dictionary_db
    ${SQLITE}
    ${JSONC}
    ${LIBZIP}
    Qt::Core
)