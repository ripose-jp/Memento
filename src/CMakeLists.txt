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

	# Add in the dylibs
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND dylibbundler -od -b -x ./MacOS/Memento -d ./Frameworks -p @executable_path/../Frameworks
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME}.app/Contents
	)

	# Manually add Python
	# Get the path to vapoursynth-script
	execute_process(
		COMMAND ./getlib.sh ${MPV_LIB} vapoursynth-script
		OUTPUT_VARIABLE OTOOL_VAPOURSYNTH_SCRIPT
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/macos
	)
	# Get the filename of vapoursynth-script mpv is linked to
	string(REGEX MATCH "[^/]*$" VAPOURSYNTH_SCRIPT_NAME ${OTOOL_VAPOURSYNTH_SCRIPT})
	# Get the otool path of Python vapoursynth-script is linked to
	execute_process(
		COMMAND ./getlib.sh ${OTOOL_VAPOURSYNTH_SCRIPT} Python
		OUTPUT_VARIABLE OTOOL_PYTHON
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/macos
	)
	# Get the python framework paths
	string(REGEX MATCH ".*Python\.framework" PYTHON_FRAMEWORK_PATH ${OTOOL_PYTHON})
	# Get the path as it is in the appbundle
	string(REGEX MATCH "Python\.framework.*" PYTHON_FRAMEWORK_END ${OTOOL_PYTHON})
	# Change the path in vapoursynth-script
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND install_name_tool -change ${OTOOL_PYTHON} @executable_path/../Frameworks/${PYTHON_FRAMEWORK_END} ${VAPOURSYNTH_SCRIPT_NAME}
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME}.app/Contents/Frameworks
	)

	# Run macdeployqt to pick up any plugins dylibbundler missed
	if (MAC_X86)
		set(MACDEPLOYQT_BIN "/usr/local/opt/qt5/bin/macdeployqt")
	else()
		set(MACDEPLOYQT_BIN macdeployqt)
	endif()
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${MACDEPLOYQT_BIN} ${PROJECT_NAME}.app
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/src
	)

	# Sign the code
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND codesign -s '${CERT}' --deep -f ${PROJECT_NAME}.app
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/src
	)

	# Copy python. This need to happen here because Python confuses codesign
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND cp -r ${PYTHON_FRAMEWORK_PATH} .
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME}.app/Contents/Frameworks
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

# This is a target that will create memento_debug.exe so that issues can be
# debugged easier on Windows
if(WIN32)
	add_executable(
		${PROJECT_NAME}_debug
		${WINDOWS_APP_ICON_RESOURCE}
		main.cpp
		../res/resources.qrc
	)
	target_link_libraries(${PROJECT_NAME}_debug mainwindow Qt5::Svg)
endif()

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
