add_library(
    audioplayer
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
    PUBLIC "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Coro>"
    PUBLIC "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Qml>"
    PUBLIC "$<$<BOOL:${SYSTEM_QCORO}>:QCoro::Network>"
    PUBLIC "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Coro>"
    PUBLIC "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Qml>"
    PUBLIC "$<$<NOT:$<BOOL:${SYSTEM_QCORO}>>:QCoro6Network>"
    PUBLIC Qt6::Core
    PUBLIC Qt6::Network
)
