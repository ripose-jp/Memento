cmake_minimum_required(VERSION 3.16)

project(
    Memento
    VERSION 0.0.0
    DESCRIPTION "An mpv-based video player for studying Japanese"
	HOMEPAGE_URL "https://ripose-jp.github.io/Memento"
    LANGUAGES C CXX
)

# Set the default build type
set(default_build_type "RelWithDebInfo")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(
		CMAKE_BUILD_TYPE "${default_build_type}" CACHE
    	STRING "Choose the type of build." FORCE
	)
  	# Set the possible values of build type for cmake-gui
  	set_property(
		CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    	"Debug" "Release" "MinSizeRel" "RelWithDebInfo"
	)
endif()

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

add_subdirectory(option)

# Check that dangerous options have no been toggled
if(NOT APPLE AND MEMENTO_APPBUNDLE)
	message(FATAL_ERROR "You cannot set the MEMENTO_APPBUNDLE option to ON on non-Apple platforms.")
endif()

# Set the module path
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Make IDEs understand the code better
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set Compiler Flags
list(
	APPEND
	MEMENTO_COMPILER_FLAGS
	"-Wall"
	"-Wextra"
	"-Wpedantic"
	"$<$<BOOL:${MEMENTO_APPBUNDLE}>:-DMEMENTO_APPBUNDLE=1>"
	"$<$<BOOL:${MEMENTO_ASAN}>:-fsanitize=address>"
	"$<$<BOOL:${MEMENTO_MECAB_SUPPORT}>:-DMEMENTO_MECAB_SUPPORT=1>"
	"$<$<BOOL:${MEMENTO_OCR_SUPPORT}>:-DMEMENTO_OCR_SUPPORT=1>"
	"$<$<BOOL:${MEMENTO_QAPPLICATION}>:-DMEMENTO_QAPPLICATION=1>"
	"$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:-DMEMENTO_SYSTEM_QCORO=1>"
	"$<$<BOOL:${MEMENTO_WERROR}>:-Werror>"
)
if(MEMENTO_ASAN)
	add_link_options(-fsanitize=address)
endif()

# Get the Git version hash
if(NOT RELEASE_BUILD)
	find_package(Git QUIET)

	if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
		execute_process(
			COMMAND ${GIT_EXECUTABLE} log --pretty=format:%h -n 1
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			OUTPUT_VARIABLE MEMENTO_VERSION_HASH
		)
	endif()
endif()

# Set the proper variables for macOS cross compilation
if(MEMENTO_MAC_CROSSCOMPILE_X86)
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()

# Generate include directories
if(APPLE AND NOT MEMENTO_MAC_CROSSCOMPILE_X86)
	# This makes sure Memento builds on whatever arch is native
	# The order of directories MATTERS because M1 users may cross compile but
	# x86_64 users will not
	set(
		MEMENTO_INCLUDE_DIRS

		# arm64 locations
		"/opt/homebrew/opt/qt6/include"
		"/opt/homebrew/include"

		# x86_64 locations
		"/usr/local/opt/qt6/include/"
		"/usr/local/include"
	)
elseif(APPLE AND MEMENTO_MAC_CROSSCOMPILE_X86)
	# This is a cross compile target, x86_64 users shouldn't need to use this
	set(
		MEMENTO_INCLUDE_DIRS
		"/usr/local/opt/qt6/include/"
		"/usr/local/include"
	)
endif()
list(PREPEND MEMENTO_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src")

# Set prefix path for macOS
if(APPLE AND NOT MEMENTO_MAC_CROSSCOMPILE_X86)
	# x86_64 locations
	list(PREPEND CMAKE_PREFIX_PATH "/usr/local/opt")
	list(PREPEND CMAKE_PREFIX_PATH "/usr/local/")

	# arm64 locations
	list(PREPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt")
	list(PREPEND CMAKE_PREFIX_PATH "/opt/homebrew")
elseif(APPLE AND MEMENTO_MAC_CROSSCOMPILE_X86)
	# x86_64 locations
	list(PREPEND CMAKE_PREFIX_PATH "/usr/local/opt")
	list(PREPEND CMAKE_PREFIX_PATH "/usr/local/")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(JsonC REQUIRED)
find_package(libzip REQUIRED)
find_package(mpv REQUIRED)
find_package(SQLite3 REQUIRED)
if(MEMENTO_SYSTEM_QCORO)
	find_package(QCoro6 REQUIRED COMPONENTS Core Network Qml)
endif()
if(MEMENTO_MECAB_SUPPORT)
	find_package(MeCab REQUIRED)
endif()
if(MEMENTO_OCR_SUPPORT AND MEMENTO_SYSTEM_MOCR)
	find_package(mocr REQUIRED)
endif()

set(
	MEMENTO_QT_COMPONENTS
	Concurrent
	Core
	Gui
	Network
	OpenGL
	Qml
	Quick
	QuickControls2
	Svg
)
if(UNIX AND NOT APPLE)
	list(APPEND MEMENTO_QT_COMPONENTS DBus)
endif()
if(MEMENTO_QAPPLICATION)
	list(APPEND MEMENTO_QT_COMPONENTS Widgets)
endif()
find_package(Qt6 REQUIRED COMPONENTS ${MEMENTO_QT_COMPONENTS})
unset(MEMENTO_QT_COMPONENTS)

qt_standard_project_setup(REQUIRES 6.8)

if(WIN32)
	add_subdirectory(windows)
endif()

add_subdirectory(extern)
add_subdirectory(src)
add_subdirectory(res)
