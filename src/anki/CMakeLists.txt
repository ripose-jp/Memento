add_library(
    anki STATIC
    ankiclient.cpp
    ankiclient.h
    ankiconfig.h
    ankiconnect.h
    glossarybuilder.cpp
    glossarybuilder.h
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
    PRIVATE mpvadapter
    PRIVATE Qt6::Concurrent
    PRIVATE Qt6::Gui
    PRIVATE subtitlelist
    PRIVATE utils
    PUBLIC "$<$<BOOL:${SYSTEM_QCORO}>:QCoro::Network>"
    PUBLIC "$<$<NOT:$<BOOL:${SYSTEM_QCORO}>>:QCoro6Network>"
    PUBLIC context
    PUBLIC Qt6::Core
    PUBLIC Qt6::Network
)
