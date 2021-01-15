cmake_minimum_required(VERSION 3.1.0)
project(memento VERSION "0.1")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
	${CMAKE_INCLUDE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)
find_library(SQLITE sqlite3)
find_library(EXPAT expat)
find_library(MPV_LIB mpv)
find_library(MECAB_LIB mecab)

add_subdirectory(src)

if(UNIX)
	install(
		TARGETS memento 
		DESTINATION bin
		COMPONENT binaries
	)
	install(
		DIRECTORY config
		DESTINATION $ENV{HOME}/.config/memento
		COMPONENT config
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