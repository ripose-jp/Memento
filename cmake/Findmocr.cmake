include(FindPackageHandleStandardArgs)

find_library(mocr_LIBRARY NAMES mocr)
find_path(mocr_INCLUDE_DIR NAMES mocr.h)

find_library(mocrxx_LIBRARY NAMES mocr++)
find_path(mocrxx_INCLUDE_DIR NAMES mocr++.h)

find_package_handle_standard_args(
    mocr
    REQUIRED_VARS
        mocr_LIBRARY mocr_INCLUDE_DIR
        mocrxx_LIBRARY mocrxx_INCLUDE_DIR
)

if(mocr_FOUND)
    mark_as_advanced(mocr_LIBRARY)
    mark_as_advanced(mocr_INCLUDE_DIR)
    mark_as_advanced(mocrxx_LIBRARY)
    mark_as_advanced(mocrxx_INCLUDE_DIR)
endif()

if(mocr_FOUND AND NOT TARGET mocr::mocr)
    add_library(mocr::mocr UNKNOWN IMPORTED)
    set_target_properties(
        mocr::mocr PROPERTIES
        IMPORTED_LOCATION "${mocr_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${mocr_INCLUDE_DIR}"
    )
endif()

if(mocr_FOUND AND NOT TARGET mocr::mocrxx)
    add_library(mocr::mocrxx UNKNOWN IMPORTED)
    set_target_properties(
        mocr::mocrxx PROPERTIES
        IMPORTED_LOCATION "${mocrxx_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${mocrxx_INCLUDE_DIR}"
    )
    target_link_libraries(mocr::mocrxx INTERFACE mocr::mocr)
endif()
