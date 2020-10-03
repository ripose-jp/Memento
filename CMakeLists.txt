cmake_minimum_required(VERSION 3.1.0)
include(GenerateExportHeader)

project(Memento)

set(CMAKE_CXX_STANDARD 14)
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