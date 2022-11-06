include(FetchContent)

FetchContent_Declare(
    libmocr
    GIT_REPOSITORY https://github.com/ripose-jp/libmocr.git
    GIT_TAG 10c1d18e09341849ce8c1bf9872382dc06339fe5
)
FetchContent_MakeAvailable(libmocr)

include_directories(${libmocr_SOURCE_DIR}/src)
add_library(
    ocrmodel
    ocrmodel.cpp
)
target_link_libraries(
    ocrmodel
    mocr++
    Qt5::Core
    Qt5::Concurrent
    Qt5::Gui
)
