add_library(
    optionswindow STATIC
    "$<$<BOOL:${OCR_SUPPORT}>:ocrsettings.cpp>"
    "$<$<BOOL:${OCR_SUPPORT}>:ocrsettings.h>"
    "$<$<BOOL:${OCR_SUPPORT}>:ocrsettings.ui>"
    ankisettings.cpp
    ankisettings.h
    ankisettings.ui
    ankisettingshelp.cpp
    ankisettingshelp.h
    ankisettingshelp.ui
    audiosourcesettings.cpp
    audiosourcesettings.h
    audiosourcesettings.ui
    behaviorsettings.cpp
    behaviorsettings.h
    behaviorsettings.ui
    cardbuilder.cpp
    cardbuilder.h
    cardbuilder.ui
    dictionarysettings.cpp
    dictionarysettings.h
    dictionarysettings.ui
    interfacesettings.cpp
    interfacesettings.h
    interfacesettings.ui
    optionswindow.cpp
    optionswindow.h
    optionswindow.ui
    searchsettings.cpp
    searchsettings.h
    searchsettings.ui
)
target_compile_features(optionswindow PUBLIC cxx_std_17)
target_compile_options(optionswindow PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(optionswindow PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    optionswindow
    PRIVATE anki
    PRIVATE dictionary_db
    PRIVATE globalmediator
    PRIVATE playeradapter
    PRIVATE scrollcombobox
    PRIVATE scrolldoublespinbox
    PUBLIC Qt6::Widgets
)
