set(DICT_SRCS
    ngram.h
    monogram.h
    digram.h
    trigram.h
    characterutil.cpp
    word.cpp
    dictionarytype.h
    dictionary.cpp
    dictionaryindexer.cpp
    dictionaryloader.cpp
    dictionarymanager.cpp)

set(DICT_LIBS
    Qt::Core
    directoryutils)

add_library(dictionary ${DICT_SRCS})
target_link_libraries(dictionary ${DICT_LIBS})