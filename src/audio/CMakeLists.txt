add_library(
    audioplayer STATIC
    audioplayer.cpp
    audioplayer.h
)
target_compile_features(audioplayer PUBLIC cxx_std_17)
target_compile_options(audioplayer PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(audioplayer PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    audioplayer
    PRIVATE globalmediator
    PRIVATE mpv::mpv
    PRIVATE Qt6::Core
    PRIVATE Qt6::Network
    PRIVATE utils
)
