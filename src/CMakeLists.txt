# Add subdirectories
add_subdirectory(anki)
add_subdirectory(audio)
add_subdirectory(constant)
add_subdirectory(definition)
add_subdirectory(dict)
add_subdirectory(manager)
add_subdirectory(os)
add_subdirectory(player)
add_subdirectory(quick)
add_subdirectory(setting)
add_subdirectory(state)
add_subdirectory(subtitle)
add_subdirectory(util)

# Executable Targets
qt_add_executable(
    memento
    main.cpp
)
if(WIN32)
    target_sources(
        memento
        PRIVATE "${PROJECT_SOURCE_DIR}/res/appicon.rc"
    )
endif()
if(APPLE AND MEMENTO_BUNDLE)
    target_sources(
        memento
        PRIVATE ${PROJECT_SOURCE_DIR}/res/memento.icns
    )
    set_source_files_properties(
        "${PROJECT_SOURCE_DIR}/res/memento.icns"
        PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
    )
endif()
configure_file(
    qml/util/Features.qml.in
    "${PROJECT_SOURCE_DIR}/src/qml/util/Features.qml"
)
set_source_files_properties(
    qml/objects/AudioSourceResolver.qml
    qml/util/AnkiMarkers.qml
    qml/util/Features.qml
    qml/util/MementoPalette.qml
    qml/util/Utils.qml
    PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE
)
qt_add_qml_module(
    memento
    URI Ripose.Memento
    VERSION 1.0
    QML_FILES
        qml/controls/AnkiFieldTextArea.qml
        qml/controls/AnkiMarkerDescription.qml
        qml/controls/AnkiNoteBox.qml
        qml/controls/KeybindCommand.qml
        qml/controls/ManualSearchPage.qml
        qml/controls/Player.qml
        qml/controls/PlayerControls.qml
        qml/controls/PlayerMenu.qml
        qml/controls/PlayerSlider.qml
        qml/controls/PlayerThumbnail.qml
        qml/controls/SearchableText.qml
        qml/controls/SelectableLabel.qml
        qml/controls/SettingsBox.qml
        qml/controls/SettingsBoxSeparator.qml
        qml/controls/SideBarList.qml
        qml/controls/StrokeLabel.qml
        qml/controls/SubtitleList.qml
        qml/controls/SubtitleListPage.qml
        qml/controls/SubtitleListTabButton.qml
        qml/controls/SubtitleText.qml
        qml/definition/DefinitionList.qml
        qml/definition/DefinitionPage.qml
        qml/definition/DefinitionPopup.qml
        qml/definition/DictionaryLabel.qml
        qml/definition/ExpressionLabel.qml
        qml/definition/FrequencyLabel.qml
        qml/definition/GlossaryText.qml
        qml/definition/KanjiEntry.qml
        qml/definition/KanjiValueTag.qml
        qml/definition/MetaLabel.qml
        qml/definition/TagLabel.qml
        qml/definition/TermEntry.qml
        qml/definition/TermGlossary.qml
        qml/definition/TermPitch.qml
        qml/dialogs/UpdateDialog.qml
        qml/Main.qml
        qml/objects/AudioFiles.qml
        qml/objects/AudioSourceResolver.qml
        qml/objects/CursorTimer.qml
        qml/objects/MoveTimer.qml
        qml/options/AnkiIntegrationHelpWindow.qml
        qml/options/AnkiIntegrationPage.qml
        qml/options/AudioSourcesPage.qml
        qml/options/BehaviorPage.qml
        qml/options/DictionaryPage.qml
        qml/options/InterfacePage.qml
        qml/options/KeybindsPage.qml
        qml/options/OcrPage.qml
        qml/options/OptionsWindow.qml
        qml/options/SearchPage.qml
        qml/util/AnkiMarkers.qml
        qml/util/Features.qml
        qml/util/MementoPalette.qml
        qml/util/Utils.qml
        qml/windows/AboutWindow.qml
)
target_compile_features(memento PRIVATE cxx_std_20)
target_include_directories(memento PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(memento PRIVATE ${MEMENTO_COMPILER_FLAGS})
set_target_properties(
    memento PROPERTIES
    OUTPUT_NAME "$<$<BOOL:${MEMENTO_BUNDLE}>:Memento>$<$<NOT:$<BOOL:${MEMENTO_BUNDLE}>>:memento>"
    MACOSX_BUNDLE_BUNDLE_NAME Memento
    MACOSX_BUNDLE_GUI_IDENTIFIER io.ripose_jp.Memento
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE ${MEMENTO_BUNDLE}
    MACOSX_BUNDLE_ICON_FILE memento.icns
    WIN32_EXECUTABLE ${MEMENTO_WIN32_EXECUTABLE}
)
target_link_libraries(
    memento
    PRIVATE "$<$<BOOL:${MEMENTO_QAPPLICATION}>:Qt6::Widgets>"
    PRIVATE "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Qml>"
    PRIVATE "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Qml>"
    PRIVATE anki
    PRIVATE audioplayer
    PRIVATE clipboard
    PRIVATE coloredsvgprovider
    PRIVATE definition
    PRIVATE dictionary
    PRIVATE keytracker
    PRIVATE managers
    PRIVATE mpvplayer
    PRIVATE Qt6::Quick
    PRIVATE Qt6::QuickControls2
    PRIVATE Qt6::Svg
    PRIVATE quickpaths
    PRIVATE screensaver
    PRIVATE settings
    PRIVATE subtitle
    PRIVATE utils
)

include(GNUInstallDirs)
install(
    TARGETS memento
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
if(APPLE AND MEMENTO_BUNDLE)
    # BundleUtilities walks the actual linked dependency graph from the bundle.
    # These directories are only search hints so it can resolve @rpath entries
    # back to the exact libraries CMake found on this machine.
    set(
        MEMENTO_BUNDLE_LIBRARY_HINTS
        "${JsonC_LIBRARY}"
        "${libzip_LIBRARY}"
        "${mpv_LIBRARY}"
    )
    if(MEMENTO_MECAB_SUPPORT)
        list(APPEND MEMENTO_BUNDLE_LIBRARY_HINTS "${MeCab_LIBRARY}")
    endif()
    if(MEMENTO_OCR_SUPPORT AND MEMENTO_SYSTEM_MOCR)
        list(APPEND MEMENTO_BUNDLE_LIBRARY_HINTS "${mocr_LIBRARY}" "${mocrxx_LIBRARY}")
    endif()
    list(REMOVE_DUPLICATES MEMENTO_BUNDLE_LIBRARY_HINTS)

    set(MEMENTO_BUNDLE_LIBRARY_DIRS "")
    foreach(MEMENTO_BUNDLE_LIBRARY_HINT IN LISTS MEMENTO_BUNDLE_LIBRARY_HINTS)
        get_filename_component(
            MEMENTO_BUNDLE_LIBRARY_DIR "${MEMENTO_BUNDLE_LIBRARY_HINT}" DIRECTORY
        )
        list(APPEND MEMENTO_BUNDLE_LIBRARY_DIRS "${MEMENTO_BUNDLE_LIBRARY_DIR}")
    endforeach()
    list(REMOVE_DUPLICATES MEMENTO_BUNDLE_LIBRARY_DIRS)

    # On macOS, use macdeployqt directly.  It is the native Qt deployment tool
    # for bundles and, unlike the generic install helper, copies QML plugins
    # into their final bundle layout instead of relying on staging symlinks.
    find_program(MEMENTO_MACDEPLOYQT_EXECUTABLE macdeployqt REQUIRED)
    configure_file(
        "${PROJECT_SOURCE_DIR}/cmake/DeployQtMacOS.cmake.in"
        "${PROJECT_BINARY_DIR}/DeployQtMacOS.cmake"
        @ONLY
    )
    install(SCRIPT "${PROJECT_BINARY_DIR}/DeployQtMacOS.cmake")

    configure_file(
        "${PROJECT_SOURCE_DIR}/cmake/BundleMacOS.cmake.in"
        "${PROJECT_BINARY_DIR}/BundleMacOS.cmake"
        @ONLY
    )
    install(SCRIPT "${PROJECT_BINARY_DIR}/BundleMacOS.cmake")

    if(MEMENTO_MECAB_SUPPORT)
        configure_file(
            "${PROJECT_SOURCE_DIR}/cmake/CopyMeCabDictionaryMacOS.cmake.in"
            "${PROJECT_BINARY_DIR}/CopyMeCabDictionaryMacOS.cmake"
            @ONLY
        )
        install(SCRIPT "${PROJECT_BINARY_DIR}/CopyMeCabDictionaryMacOS.cmake")
    endif()

    add_custom_target(
        memento_bundle
        COMMAND
            "${CMAKE_COMMAND}"
            -DMEMENTO_BUNDLE_PATH=$<TARGET_BUNDLE_DIR:memento>
            -P "${PROJECT_BINARY_DIR}/DeployQtMacOS.cmake"
        COMMAND
            "${CMAKE_COMMAND}"
            -DMEMENTO_BUNDLE_PATH=$<TARGET_BUNDLE_DIR:memento>
            -P "${PROJECT_BINARY_DIR}/BundleMacOS.cmake"
        DEPENDS memento
        COMMENT "Bundling dependencies into Memento.app"
        VERBATIM
    )
    if(MEMENTO_MECAB_SUPPORT)
        add_custom_command(
            TARGET memento_bundle
            POST_BUILD
            COMMAND
                "${CMAKE_COMMAND}"
                -DMEMENTO_BUNDLE_PATH=$<TARGET_BUNDLE_DIR:memento>
                -P "${PROJECT_BINARY_DIR}/CopyMeCabDictionaryMacOS.cmake"
            VERBATIM
        )
    endif()

    if(MEMENTO_CODESIGN_IDENTITY)
        configure_file(
            "${PROJECT_SOURCE_DIR}/cmake/SignMacOS.cmake.in"
            "${PROJECT_BINARY_DIR}/SignMacOS.cmake"
            @ONLY
        )
        install(SCRIPT "${PROJECT_BINARY_DIR}/SignMacOS.cmake")
        add_custom_command(
            TARGET memento_bundle
            POST_BUILD
            COMMAND
                "${CMAKE_COMMAND}"
                -DMEMENTO_BUNDLE_PATH=$<TARGET_BUNDLE_DIR:memento>
                -P "${PROJECT_BINARY_DIR}/SignMacOS.cmake"
            VERBATIM
        )
    endif()
endif()
if(UNIX AND NOT APPLE)
	install(
		FILES "${PROJECT_SOURCE_DIR}/res/memento.desktop"
		DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
	)
	install(
		FILES "${PROJECT_SOURCE_DIR}/res/memento.svg"
		DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
	)
endif()
