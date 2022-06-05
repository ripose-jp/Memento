cmake_minimum_required(VERSION 3.1.0)

set(MEMENTO_VERSION "0.5.5-1-beta")
set(MEMENTO_SHORT_VERSION "0.5.5.1")
execute_process(
    COMMAND git log --pretty=format:'%h' -n 1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE MEMENTO_VERSION_HASH
)

if(APPLE)
	project(Memento VERSION ${MEMENTO_SHORT_VERSION})
else()
	project(memento VERSION ${MEMENTO_SHORT_VERSION})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCMAKE_VERSION='\"${MEMENTO_VERSION}\"'")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCMAKE_VERSION='\"${MEMENTO_VERSION}\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Add the hash to the version if the RELEASE_BUILD flag isn't set
if(NOT RELEASE_BUILD)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCMAKE_HASH='\"${MEMENTO_VERSION_HASH}\"'")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCMAKE_HASH='\"${MEMENTO_VERSION_HASH}\"'")
endif()

if(APPIMAGE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAPPIMAGE=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPPIMAGE=1")
endif()

if(APPBUNDLE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAPPBUNDLE=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPPBUNDLE=1")
endif()

# Sets the include directories to src
# This isn't best CMake practice, but it produces the desired behavior of
# setting include directories for all targets
set(INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories(${INCLUDE_DIRECTORY})

if(APPLE AND NOT MAC_X86)
	# This makes sure Memento builds on whatever arch is native
	# The order of directories MATTERS because M1 users may crosscompile but
	# x86_64 users will not
	include_directories(
		${CMAKE_INCLUDE_PATH}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}

		# arm64 locations
		/opt/homebrew/include
		/opt/homebrew/opt/qt5/include

		# x86_64 locations
		/usr/local/include
		/usr/local/opt/qt5/include/
	)

	# x86_64 locations
	list(PREPEND CMAKE_PREFIX_PATH /usr/local/opt)
	list(PREPEND CMAKE_PREFIX_PATH /usr/local/)

	# arm64 locations
	list(PREPEND CMAKE_PREFIX_PATH /opt/homebrew/opt)
	list(PREPEND CMAKE_PREFIX_PATH /opt/homebrew)
elseif(APPLE AND MAC_X86)
	# This is a crosscomile target, x86_64 users shouldn't need to use this
	include_directories(
		${CMAKE_INCLUDE_PATH}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		/usr/local/include
		/usr/local/opt/qt5/include/
	)

	list(PREPEND CMAKE_PREFIX_PATH /usr/local/opt)
	list(PREPEND CMAKE_PREFIX_PATH /usr/local/)
else()
	include_directories(
		${CMAKE_INCLUDE_PATH}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
	)
endif()

# Find Qt
if(UNIX AND NOT APPLE)
	find_package(Qt5 COMPONENTS Widgets Network DBus X11Extras Svg REQUIRED)
	include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})
elseif(APPLE)
	find_package(Qt5 COMPONENTS Widgets Network Svg REQUIRED)
elseif(WIN32)
	find_package(Qt5 COMPONENTS Widgets Network Svg REQUIRED)
endif()

# Other dependencies
find_library(MECAB mecab)
find_library(SQLITE sqlite3)
find_library(MPV_LIB mpv)
find_library(JSONC json-c)
find_library(LIBZIP zip)

add_subdirectory(src)
