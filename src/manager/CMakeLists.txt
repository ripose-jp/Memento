add_library(
    managers
    mainmanager.cpp
    mainmanager.h
    playermanager.cpp
    playermanager.h
    subtitlelistmanager.cpp
    subtitlelistmanager.h
)
target_compile_features(managers PRIVATE cxx_std_20)
target_include_directories(managers PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(managers PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    managers
    PRIVATE context
    PRIVATE dictionary
    PRIVATE mpvplayer
    PRIVATE Qt6::Concurrent
    PRIVATE subtitle
    PUBLIC "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Coro>"
    PUBLIC "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Coro>"
    PUBLIC Qt6::Core
    PUBLIC Qt6::Qml
)
