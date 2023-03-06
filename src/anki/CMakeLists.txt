add_library(
    anki STATIC
    ankiclient.cpp
    ankiclient.h
    ankiconfig.h
    glossarybuilder.h
    glossarybuilder.cpp
)
target_compile_features(anki PUBLIC cxx_std_17)
target_compile_options(anki PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(anki PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    anki
    PRIVATE globalmediator
    PRIVATE mpvadapter
    PRIVATE Qt5::Gui
    PRIVATE Qt5::Network
    PRIVATE subtitlelist
    PRIVATE utils
    PUBLIC Qt5::Core
)
