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
    fileutils
    Qt5::Core
    Qt5::Network
)