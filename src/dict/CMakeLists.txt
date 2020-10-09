find_library(SQLITE sqlite3)

set(DICT_SRCS
    sqlite.h
    entry.h
    jmdict.cpp)

add_library(jmdict ${DICT_SRCS})
target_link_libraries(jmdict
    ${SQLITE}
    Qt::Core)