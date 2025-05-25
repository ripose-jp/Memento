add_library(
    definitionwidget STATIC
    definitionstate.h
    definitionwidget.cpp
    definitionwidget.h
    definitionwidget.ui
    glossarylabel.cpp
    glossarylabel.h
    glossarywidget.cpp
    glossarywidget.h
    kanjiwidget.cpp
    kanjiwidget.h
    pitchwidget.cpp
    pitchwidget.h
    tagwidget.cpp
    tagwidget.h
    termwidget.cpp
    termwidget.h
    termwidget.ui
)
target_compile_features(definitionwidget PUBLIC cxx_std_20)
target_compile_options(definitionwidget PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(definitionwidget PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    definitionwidget
    PRIVATE audioplayer
    PRIVATE constants
    PRIVATE dictionary_db
    PRIVATE flowlayout
    PRIVATE iconfactory
    PRIVATE playeradapter
    PRIVATE QCoro6Network
    PRIVATE Qt6::Network
    PRIVATE subtitlelist
    PRIVATE utils
    PUBLIC anki
    PUBLIC context
    PUBLIC QCoro6Core
    PUBLIC Qt6::Widgets
)
