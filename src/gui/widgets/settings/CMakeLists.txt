add_library(
    ankisettings
    ankisettings.cpp
    ankisettings.ui
    cardbuilder.cpp
    cardbuilder.ui
    ankisettingshelp.cpp
    ankisettingshelp.ui
)
target_link_libraries(
    ankisettings
    globalmediator
    Qt5::Widgets
    anki
    scrollcombobox
    scrolldoublespinbox
)
list(APPEND OPTION_WINDOW_LIBS ankisettings)

add_library(
    audiosourcesettings
    audiosourcesettings.cpp
    audiosourcesettings.ui
)
target_link_libraries(
    audiosourcesettings
    globalmediator
    Qt5::Widgets
)
list(APPEND OPTION_WINDOW_LIBS audiosourcesettings)

add_library(
    behaviorsettings
    behaviorsettings.cpp
    behaviorsettings.ui
)
target_link_libraries(
    behaviorsettings
    globalmediator
    Qt5::Widgets
)
list(APPEND OPTION_WINDOW_LIBS behaviorsettings)

add_library(
    dictionarysettings
    dictionarysettings.cpp
    dictionarysettings.ui
)
target_link_libraries(
    dictionarysettings
    globalmediator
    Qt5::Widgets
    dictionary_db
)
list(APPEND OPTION_WINDOW_LIBS dictionarysettings)

add_library(
    searchsettings
    searchsettings.cpp
    searchsettings.ui
)
target_link_libraries(
    searchsettings
    Qt5::Widgets
    Qt5::Core
)
list(APPEND OPTION_WINDOW_LIBS searchsettings)

add_library(
    interfacesettings
    interfacesettings.cpp
    interfacesettings.ui
)
target_link_libraries(
    interfacesettings
    Qt5::Widgets
    Qt5::Core
)
list(APPEND OPTION_WINDOW_LIBS interfacesettings)

if(OCR_SUPPORT)
    add_library(
        ocrsettings
        ocrsettings.cpp
        ocrsettings.ui
    )
    target_link_libraries(
        ocrsettings
        Qt5::Widgets
        Qt5::Core
    )
    list(APPEND OPTION_WINDOW_LIBS ocrsettings)
endif()

add_library(
    optionswindow
    optionswindow.cpp
    optionswindow.ui
)
target_link_libraries(
    optionswindow
    ${OPTION_WINDOW_LIBS}
)