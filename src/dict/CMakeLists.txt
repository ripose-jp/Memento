set(
    DICT_SRCS
    sqlite.h
    entry.h
    jmdict.cpp
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
    ${SQLITE}
    Qt::Core
)

add_library(jmdict_builder ${BUILDER_SRCS})
target_link_libraries(
    jmdict_builder
    ${SQLITE}
    ${EXPAT}
)