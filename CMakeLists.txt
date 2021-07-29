cmake_minimum_required(VERSION 3.1.0)
project(memento VERSION "0.4.7.1")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(UNIX AND APPLE)
	include_directories(
		${CMAKE_INCLUDE_PATH}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
		/usr/local/include/
	)
	set(CMAKE_PREFIX_PATH /usr/local/opt/)
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

# OS X Links the non-brew versions
if(UNIX AND APPLE)
	set(SQLITE "/usr/local/opt/sqlite3/lib/libsqlite3.dylib")
	set(MECAB "/usr/local/opt/mecab/lib/libmecab.dylib")
else()
	find_library(SQLITE sqlite3)
	find_library(MECAB mecab)
endif()

# Other dependencies
find_library(MPV_LIB mpv)
find_library(JSONC json-c)
find_library(LIBZIP zip)

add_subdirectory(src)

if(UNIX AND NOT APPLE)
	install(
		TARGETS memento 
		DESTINATION bin
		COMPONENT binaries
	)
	install(
		FILES res/memento.desktop
		DESTINATION share/applications
	)
	install(
		FILES res/memento.svg
		DESTINATION share/icons/hicolor/scalable/apps
	)
endif()