cmake_minimum_required(VERSION 3.15...3.25)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(
	Memento
	VERSION 1.4.1
	DESCRIPTION "An mpv-based video player for studying Japanese"
	HOMEPAGE_URL "https://ripose-jp.github.io/Memento"
	LANGUAGES CXX C
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

# Options
add_subdirectory(option)

# Check that dangerous options have no been toggled
if(NOT APPLE AND APPBUNDLE)
	message(FATAL_ERROR "You cannot set the APPBUNDLE option to ON on non-Apple platforms.")
endif()
if(UNIX AND NOT APPLE AND APPIMAGE)
	message(FATAL_ERROR "You cannot set the APPIMAGE option to ON on non-Unix or Apple platforms.")
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
	"$<$<BOOL:${APPIMAGE}>:-DAPPIMAGE=1>"
	"$<$<BOOL:${APPBUNDLE}>:-DAPPBUNDLE=1>"
	"$<$<BOOL:${OCR_SUPPORT}>:-DOCR_SUPPORT=1>"
	"$<$<BOOL:${MECAB_SUPPORT}>:-DMECAB_SUPPORT=1>"
)

# Set Qt preprocessor settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

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

# Set macOS deployment target
if(RELEASE_BUILD AND APPLE)
	set(MACOSX_DEPLOYMENT_TARGET "11.0")
endif()

# Set the proper variables for macOS cross compilation
if(MAC_CROSSCOMPILE_X86)
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()

# Generate include directories
if(APPLE AND NOT MAC_CROSSCOMPILE_X86)
	# This makes sure Memento builds on whatever arch is native
	# The order of directories MATTERS because M1 users may crosscompile but
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
elseif(APPLE AND MAC_CROSSCOMPILE_X86)
	# This is a crosscomile target, x86_64 users shouldn't need to use this
	set(
		MEMENTO_INCLUDE_DIRS
		"/usr/local/opt/qt6/include/"
		"/usr/local/include"
	)
endif()
list(PREPEND MEMENTO_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src")

# Set prefix path for macOS
if(APPLE AND NOT MAC_CROSSCOMPILE_X86)
	# x86_64 locations
	list(PREPEND CMAKE_PREFIX_PATH "/usr/local/opt")
	list(PREPEND CMAKE_PREFIX_PATH "/usr/local/")

	# arm64 locations
	list(PREPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt")
	list(PREPEND CMAKE_PREFIX_PATH "/opt/homebrew")
elseif(APPLE AND MAC_CROSSCOMPILE_X86)
	# x86_64 locations
	list(PREPEND CMAKE_PREFIX_PATH "/usr/local/opt")
	list(PREPEND CMAKE_PREFIX_PATH "/usr/local/")
endif()

# Dependencies
find_package(JsonC REQUIRED)
find_package(libzip REQUIRED)
if(MECAB_SUPPORT)
	find_package(MeCab REQUIRED)
endif()
find_package(mpv REQUIRED)
find_package(SQLite3 REQUIRED)
if (UNIX AND NOT APPLE)
	find_package(
		Qt6 REQUIRED
		COMPONENTS
			Concurrent
			DBus
			Gui
			Network
			OpenGLWidgets
			Svg
			Widgets
	)
else()
	find_package(
		Qt6 REQUIRED
		COMPONENTS
			Concurrent
			Gui
			Network
			OpenGLWidgets
			Svg
			Widgets
	)
endif()

# Include CMake Modules
include(FetchContent)

# Subdirectories
add_subdirectory(extern)
add_subdirectory(src)
