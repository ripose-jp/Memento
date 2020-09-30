set(DICT_SRCS
    characterutil.cpp
    word.cpp)

set(DICT_LIBS
    Qt::Core)

add_library(dictionary ${DICT_SRCS})
target_link_libraries(dictionary ${DICT_LIBS})