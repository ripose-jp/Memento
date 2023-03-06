if(OCR_SUPPORT)
    FetchContent_Declare(
        libmocr
        GIT_REPOSITORY https://github.com/ripose-jp/libmocr.git
        GIT_TAG 6b16b097e67abaad068f33ccba8442929a803237
    )
    FetchContent_MakeAvailable(libmocr)
endif()
