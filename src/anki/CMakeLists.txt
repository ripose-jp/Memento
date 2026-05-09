add_library(
    anki
    ankiclient.cpp
    ankiclient.h
    ankiconfig.cpp
    ankiconfig.h
    ankiconfigkeys.h
    ankiconnect.h
    ankifield.h
    ankifieldlistmodel.cpp
    ankifieldlistmodel.h
    ankiprofile.cpp
    ankiprofile.h
    glossarybuilder.cpp
    glossarybuilder.h
    marker.h
    markertokenizer.cpp
    markertokenizer.h
    notebuilder.cpp
    notebuilder.h
)
target_compile_features(anki PUBLIC cxx_std_20)
target_compile_options(anki PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(anki PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    anki
    PRIVATE "$<$<BOOL:${SYSTEM_QCORO}>:QCoro::Network>"
    PRIVATE "$<$<NOT:$<BOOL:${SYSTEM_QCORO}>>:QCoro6Network>"
    PRIVATE context
    PRIVATE Qt6::Concurrent
    PRIVATE utils
    PUBLIC "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Coro>"
    PUBLIC "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Qml>"
    PUBLIC "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Coro>"
    PUBLIC "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Qml>"
    PUBLIC Qt6::Core
    PUBLIC Qt6::Network
)
