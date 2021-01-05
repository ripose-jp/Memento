set(
    ANKI_SRCS
    ankiclient.cpp
)

add_library(anki ${ANKI_SRCS})
target_link_libraries(
    anki
    Qt::Core
    Qt5::Network
)