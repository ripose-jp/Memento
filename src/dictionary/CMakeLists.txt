set(DICT_SRCS
    characterutil.cpp
    word.cpp
    ngram.h
    monogram.h
    digram.h
    trigram.h
    dictionary.cpp)

set(DICT_LIBS
    Qt::Core)

add_library(dictionary ${DICT_SRCS})
target_link_libraries(dictionary ${DICT_LIBS})