cmake_minimum_required(VERSION 3.1.0)
project(Memento VERSION "0.1")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
	${CMAKE_INCLUDE_PATH}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_subdirectory(src)