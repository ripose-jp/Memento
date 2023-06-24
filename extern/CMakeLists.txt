if(OCR_SUPPORT)
    FetchContent_Declare(
        libmocr
        GIT_REPOSITORY https://github.com/ripose-jp/libmocr.git
        GIT_TAG 24ec081102208d0ff6954c3003df2fb691713bd5
    )
    FetchContent_MakeAvailable(libmocr)
endif()
