if(NOT SYSTEM_QCORO)
    set(QCORO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(QCORO_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(QCORO_WITH_QTDBUS OFF CACHE BOOL "" FORCE)
    set(QCORO_WITH_QTNETWORK ON CACHE BOOL "" FORCE)
    set(QCORO_WITH_QTWEBSOCKETS OFF CACHE BOOL "" FORCE)
    set(QCORO_WITH_QTQUICK OFF CACHE BOOL "" FORCE)
    set(QCORO_WITH_QML OFF CACHE BOOL "" FORCE)
    set(QCORO_WITH_QTTEST OFF CACHE BOOL "" FORCE)

    set(MEMENTO_QCORO_PATCH_PATH "${CMAKE_CURRENT_SOURCE_DIR}/patch/qcoro-remove-install.patch")
    set(MEMENTO_QCORO_PATCH_COMMAND git clean -fd && git reset --hard && git apply ${MEMENTO_QCORO_PATCH_PATH})
    if(WIN32)
        set(MEMENTO_QCORO_PATCH_COMMAND "")
    endif()
    FetchContent_Declare(
        qcoro
        GIT_REPOSITORY https://github.com/qcoro/qcoro.git
        GIT_TAG ef1231ae490465b4f592ca326eee6d56a3d802ae # v0.12.0
        PATCH_COMMAND ${MEMENTO_QCORO_PATCH_COMMAND}
    )
    FetchContent_MakeAvailable(qcoro)
endif()

if(OCR_SUPPORT AND NOT SYSTEM_MOCR)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        libmocr
        GIT_REPOSITORY https://github.com/ripose-jp/libmocr.git
        GIT_TAG 24ec081102208d0ff6954c3003df2fb691713bd5
    )
    FetchContent_MakeAvailable(libmocr)
endif()
