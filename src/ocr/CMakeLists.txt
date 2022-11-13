include(FetchContent)

FetchContent_Declare(
    libmocr
    GIT_REPOSITORY https://github.com/ripose-jp/libmocr.git
    GIT_TAG 6b01c9c753fa4b9e44a71ef5fcdbef66f21d77b2
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
