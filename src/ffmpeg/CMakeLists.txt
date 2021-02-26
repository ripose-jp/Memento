add_library(
    transcoder
    transcode_aac.c
)
target_link_libraries(
    transcoder
    ${FFMPEG}
)