cmake_minimum_required(VERSION 3.1.0)

set(MEMENTO_VERSION "0.5.0-1-beta")
set(MEMENTO_SHORT_VERSION "0.5.0.1")
if(APPLE)
	project(Memento VERSION ${MEMENTO_SHORT_VERSION})
else()
	project(memento VERSION ${MEMENTO_SHORT_VERSION})
endif()

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
