add_subdirectory(util)
add_subdirectory(audio)
add_subdirectory(dict)
add_subdirectory(gui)
add_subdirectory(anki)

if(WIN32)
	set(WINDOWS_APP_ICON_RESOURCE ../res/appicon.rc)
    set(WINDOWS_EXE_OPTION WIN32)
endif()

if(APPLE AND APPBUNDLE)
	set(MACOS_ICON_FILE memento.icns)
	set(MACOS_ICON_PATH ${PROJECT_SOURCE_DIR}/res/${MACOS_ICON_FILE})

	add_executable(
		${PROJECT_NAME}
		MACOSX_BUNDLE
		main.cpp
		../res/resources.qrc
		${MACOS_ICON_PATH}
	)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		BUNDLE True
		MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
		MACOSX_BUNDLE_BUNDLE_VERSION ${MEMENTO_VERSION}
		MACOSX_BUNDLE_COPYRIGHT "2020 - 2021 Ripose"
		MACOSX_BUNDLE_GUI_IDENTIFIER "io.github.ripose-jp.memento"
		MACOSX_BUNDLE_ICON_FILE ${MACOS_ICON_FILE}
		MACOSX_BUNDLE_INFO_STRING "An mpv-based video player for studying Japanese"
		MACOSX_BUNDLE_LONG_VERSION_STRING ${MEMENTO_VERSION}
		MACOSX_BUNDLE_SHORT_VERSION_STRING ${MEMENTO_SHORT_VERSION}
		MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/macos/MementoInfo.plist.in
		RESOURCE ${MACOS_ICON_PATH}
	)
	# Must call the correct macdeployqt when compiling
	if (MAC_X86)
		set(MACDEPLOYQT_BIN "/usr/local/opt/qt5/bin/macdeployqt")
	else()
		set(MACDEPLOYQT_BIN macdeployqt)
	endif()
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${MACDEPLOYQT_BIN} ${PROJECT_NAME}.app -codesign=${CERT}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/src
	)
else()
	add_executable(
		${PROJECT_NAME}
		${WINDOWS_EXE_OPTION}
		${WINDOWS_APP_ICON_RESOURCE}
		main.cpp
		../res/resources.qrc
	)
endif()
target_link_libraries(${PROJECT_NAME} mainwindow Qt5::Svg)

# Install targets
if(UNIX AND NOT APPLE)
	install(
		TARGETS ${PROJECT_NAME}
		DESTINATION bin
		COMPONENT binaries
	)
	install(
		FILES ${PROJECT_SOURCE_DIR}/res/memento.desktop
		DESTINATION share/applications
	)
	install(
		FILES ${PROJECT_SOURCE_DIR}/res/memento.svg
		DESTINATION share/icons/hicolor/scalable/apps
	)
endif()
