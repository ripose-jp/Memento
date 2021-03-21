set(
    ANKI_SRCS
    ankiclient.cpp
    ankiconfig.h
)

add_library(anki ${ANKI_SRCS})
target_link_libraries(
    anki
    globalmediator
    mpvadapter
    transcoder
    Qt::Core
    Qt5::Network
)