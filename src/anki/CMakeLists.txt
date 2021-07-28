set(
    ANKI_SRCS
    ankiclient.cpp
    ankiconfig.h
)

add_library(anki ${ANKI_SRCS})
target_link_libraries(
    anki
    utils
    globalmediator
    mpvadapter
    Qt5::Core
    Qt5::Network
)