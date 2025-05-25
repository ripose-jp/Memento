add_library(
    audioplayer STATIC
    audioplayer.cpp
    audioplayer.h
)
target_compile_features(audioplayer PUBLIC cxx_std_20)
target_compile_options(audioplayer PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(audioplayer PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    audioplayer
    PRIVATE mpv::mpv
    PRIVATE utils
    PUBLIC context
    PUBLIC QCoro6Network
    PUBLIC Qt6::Core
    PUBLIC Qt6::Network
)
