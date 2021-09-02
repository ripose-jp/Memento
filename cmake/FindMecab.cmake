# CMake module for finding mecab

# Look for the header file
if(APPLE)
    set(MECAB_INCLUDE_DIR "/opt/homebrew/include/mecab.h")
    set(MECAB_LIBRARY "/opt/homebrew/lib/libmecab.dylib")
else()
    find_path(
        MECAB_INCLUDE_DIR
        NAMES mecab.h
    )
    find_library(
        MECAB_LIBRARY
        NAMES mecab libmecab
    )
endif()

include(FindPackageHandleStandardArgs)
