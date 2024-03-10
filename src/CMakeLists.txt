# Create version header
configure_file(
    "${PROJECT_SOURCE_DIR}/src/version.h.in"
    "${PROJECT_BINARY_DIR}/src/version.h"
)
add_library(version STATIC "${PROJECT_BINARY_DIR}/src/version.h")
target_compile_features(version PUBLIC cxx_std_17)
target_compile_options(version PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(version PUBLIC "${PROJECT_BINARY_DIR}/src")

# Add subdirectories
add_subdirectory(anki)
add_subdirectory(audio)
add_subdirectory(dict)
add_subdirectory(gui)
add_subdirectory(macos)
add_subdirectory(ocr)
add_subdirectory(player)
add_subdirectory(util)

# Executable Targets
if(APPBUNDLE)
	set(MEMENTO_ICON_FILE "memento.icns")
	set(MEMENTO_ICON_PATH "${PROJECT_SOURCE_DIR}/res/${MEMENTO_ICON_FILE}")

	# Create the executable target
	add_executable(
		${PROJECT_NAME} MACOSX_BUNDLE
		main.cpp
		"${PROJECT_SOURCE_DIR}/res/resources.qrc"
		"${MEMENTO_ICON_PATH}"
	)
	target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
	target_compile_options(${PROJECT_NAME} PRIVATE ${MEMENTO_COMPILER_FLAGS})
	target_include_directories(${PROJECT_NAME} PRIVATE ${MEMENTO_INCLUDE_DIRS})
	set_target_properties(
		${PROJECT_NAME} PROPERTIES
		BUNDLE True
		MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
		MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
		MACOSX_BUNDLE_COPYRIGHT "2020 - 2024 Ripose"
		MACOSX_BUNDLE_GUI_IDENTIFIER "io.github.ripose-jp.memento"
		MACOSX_BUNDLE_ICON_FILE ${MEMENTO_ICON_FILE}
		MACOSX_BUNDLE_INFO_STRING ${PROJECT_DESCRIPTION}
		MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION}
		MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
		MACOSX_BUNDLE_INFO_PLIST "${PROJECT_SOURCE_DIR}/macos/MementoInfo.plist.in"
		RESOURCE ${MEMENTO_ICON_PATH}
	)
	target_link_libraries(
		${PROJECT_NAME}
		PRIVATE audioplayer
		PRIVATE dictionary_db
		PRIVATE globalmediator
		PRIVATE iconfactory
		PRIVATE mainwindow
		PRIVATE Qt6::Svg
	)

	# Add in the dylibs
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND dylibbundler -od -b -x ./MacOS/Memento -d ./Frameworks -p @executable_path/../Frameworks
		WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/src/${PROJECT_NAME}.app/Contents"
	)

	# Manually add Python
	# Get the path to vapoursynth-script
	execute_process(
		COMMAND ./getlib.sh "${mpv_LIBRARY}" vapoursynth-script
		OUTPUT_VARIABLE OTOOL_VAPOURSYNTH_SCRIPT
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/macos"
	)
	# Get the filename of vapoursynth-script mpv is linked to
	string(REGEX MATCH "[^/]*$" VAPOURSYNTH_SCRIPT_NAME ${OTOOL_VAPOURSYNTH_SCRIPT})
	# Get the otool path of Python vapoursynth-script is linked to
	execute_process(
		COMMAND ./getlib.sh "${OTOOL_VAPOURSYNTH_SCRIPT}" Python
		OUTPUT_VARIABLE OTOOL_PYTHON
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/macos"
	)
	# Get the python framework paths
	string(REGEX MATCH ".*Python\.framework/Versions/.*/" PYTHON_FRAMEWORK_PATH ${OTOOL_PYTHON})
	# Change the path in vapoursynth-script
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND install_name_tool -change "${OTOOL_PYTHON}" "@executable_path/../Frameworks/Python.framework/Python" "${VAPOURSYNTH_SCRIPT_NAME}"
		WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/src/${PROJECT_NAME}.app/Contents/Frameworks"
	)

	# Run macdeployqt to pick up any plugins dylibbundler missed
	if (MAC_CROSSCOMPILE_X86)
		set(MACDEPLOYQT_BIN "/usr/local/opt/qt6/bin/macdeployqt")
	else()
		set(MACDEPLOYQT_BIN macdeployqt)
	endif()
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${MACDEPLOYQT_BIN} ${PROJECT_NAME}.app
		WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/src"
	)

	# Copy python. This need to happen here because Python confuses codesign
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND mkdir Python.framework && cp -RL "${PYTHON_FRAMEWORK_PATH}/*" "Python.framework" && chmod -R 755 Python.framework
		WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/src/${PROJECT_NAME}.app/Contents/Frameworks"
	)
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND xattr -cr "${PROJECT_NAME}.app"
		WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/src"
	)

	# Sign the code
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND codesign -s '${CERT}' --deep -f ${PROJECT_NAME}.app
		WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/src"
	)
elseif(WIN32)
	add_executable(
		memento WIN32
		main.cpp
		"${PROJECT_SOURCE_DIR}/res/resources.qrc"
		"${PROJECT_SOURCE_DIR}/res/appicon.rc"
	)
	target_compile_features(memento PUBLIC cxx_std_17)
	target_compile_options(memento PRIVATE ${MEMENTO_COMPILER_FLAGS})
	target_include_directories(memento PRIVATE ${MEMENTO_INCLUDE_DIRS})
	target_link_libraries(
		memento
		PRIVATE audioplayer
		PRIVATE dictionary_db
		PRIVATE globalmediator
		PRIVATE iconfactory
		PRIVATE mainwindow
		PRIVATE Qt6::Svg
	)

	# This is a target that will create memento_debug.exe so that issues can be
	# debugged easier on Windows
	add_executable(
		memento_debug
		main.cpp
		"${PROJECT_SOURCE_DIR}/res/resources.qrc"
		"${PROJECT_SOURCE_DIR}/res/appicon.rc"
	)
	target_compile_features(memento_debug PUBLIC cxx_std_17)
	target_compile_options(memento_debug PRIVATE ${MEMENTO_COMPILER_FLAGS})
	target_include_directories(memento_debug PRIVATE ${MEMENTO_INCLUDE_DIRS})
	target_link_libraries(
		memento_debug
		PRIVATE audioplayer
		PRIVATE dictionary_db
		PRIVATE globalmediator
		PRIVATE iconfactory
		PRIVATE mainwindow
		PRIVATE Qt6::Svg
	)
elseif(UNIX)
	add_executable(
		memento
		main.cpp
		"${PROJECT_SOURCE_DIR}/res/resources.qrc"
	)
	target_compile_features(memento PUBLIC cxx_std_17)
	target_compile_options(memento PRIVATE ${MEMENTO_COMPILER_FLAGS})
	target_include_directories(memento PRIVATE ${MEMENTO_INCLUDE_DIRS})
	target_link_libraries(
		memento
		PRIVATE audioplayer
		PRIVATE dictionary_db
		PRIVATE globalmediator
		PRIVATE iconfactory
		PRIVATE mainwindow
		PRIVATE Qt6::Svg
	)
else()
	message(FATAL_ERROR "Unsupported operating system!")
endif()

# Install targets
if(NOT APPBUNDLE AND NOT APPIMAGE)
	install(
		TARGETS memento
		DESTINATION bin
		COMPONENT binaries
	)
endif()
if(UNIX AND NOT APPLE)
	install(
		FILES "${PROJECT_SOURCE_DIR}/res/memento.desktop"
		DESTINATION share/applications
	)
	install(
		FILES "${PROJECT_SOURCE_DIR}/res/memento.svg"
		DESTINATION share/icons/hicolor/scalable/apps
	)
endif()
