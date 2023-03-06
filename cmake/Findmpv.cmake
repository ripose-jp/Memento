include(FindPackageHandleStandardArgs)

find_library(mpv_LIBRARY NAMES mpv)
find_path(mpv_INCLUDE_DIR NAMES mpv/client.h mpv/render_gl.h)

find_package_handle_standard_args(
    mpv
    REQUIRED_VARS mpv_LIBRARY mpv_INCLUDE_DIR
)

if(mpv_FOUND)
    mark_as_advanced(mpv_LIBRARY)
    mark_as_advanced(mpv_INCLUDE_DIR)
endif()

if(mpv_FOUND AND NOT TARGET mpv::mpv)
    add_library(mpv::mpv UNKNOWN IMPORTED)
    set_target_properties(
        mpv::mpv PROPERTIES
        IMPORTED_LOCATION "${mpv_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${mpv_INCLUDE_DIR}"
    )
endif()
