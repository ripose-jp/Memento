cmake_minimum_required(VERSION 3.1.0)
project(memento VERSION "0.5.0.1")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(APPIMAGE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAPPIMAGE=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DAPPIMAGE=1")
endif()

if(APPLE)
	include_directories(
		${CMAKE_INCLUDE_PATH}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		/opt/homebrew/include
		/opt/homebrew/opt/qt5/include
	)

	list(PREPEND CMAKE_PREFIX_PATH /opt/homebrew/opt)
	list(PREPEND CMAKE_PREFIX_PATH /opt/homebrew)
else()
	include_directories(
		${CMAKE_INCLUDE_PATH}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
	)
endif()

# Find Qt
if(UNIX AND NOT APPLE)
	find_package(Qt5 COMPONENTS Widgets Network DBus REQUIRED)
elseif(UNIX AND APPLE)
	find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
elseif(WIN32)
	find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
endif()

# Other dependencies
if(APPLE)
	set(MECAB "/opt/homebrew/lib/libmecab.dylib")
else()
	find_library(MECAB mecab)
endif()
find_library(SQLITE sqlite3)
find_library(MPV_LIB mpv)
find_library(JSONC json-c)
find_library(LIBZIP zip)

add_subdirectory(src)
