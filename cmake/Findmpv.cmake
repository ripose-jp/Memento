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

if(mpv_FOUND AND WIN32)
    include(CheckCXXSourceCompiles)
    include(CMakePushCheckState)
    cmake_push_check_state(RESET)
    set(CMAKE_REQUIRED_INCLUDES "${mpv_INCLUDE_DIR}")
    check_cxx_source_compiles([[
        #include <mpv/render_d3d11.h>
        #if MPV_CLIENT_API_VERSION < MPV_MAKE_VERSION(2, 6)
        #error Windows requires mpv 0.42 or newer.
        #endif
        /* Verifies the native D3D11 render API without requiring a GPU. */
        int main()
        {
            (void) MPV_RENDER_API_TYPE_D3D11;
            (void) MPV_RENDER_PARAM_D3D11_INIT_PARAMS;
            (void) MPV_RENDER_PARAM_D3D11_FBO;
            (void) sizeof(mpv_d3d11_init_params);
            (void) sizeof(mpv_d3d11_fbo);
            return 0;
        }
    ]] mpv_HAS_D3D11)
    cmake_pop_check_state()
    if(NOT mpv_HAS_D3D11)
        message(FATAL_ERROR
            "Windows requires mpv 0.42 or newer with the D3D11 render API. "
            "Update the mpv development package and runtime library."
        )
    endif()
endif()

if(mpv_FOUND AND NOT TARGET mpv::mpv)
    add_library(mpv::mpv UNKNOWN IMPORTED)
    set_target_properties(
        mpv::mpv PROPERTIES
        IMPORTED_LOCATION "${mpv_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${mpv_INCLUDE_DIR}"
    )
endif()
