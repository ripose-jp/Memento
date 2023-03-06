include(FindPackageHandleStandardArgs)

find_library(JsonC_LIBRARY NAMES json-c)
find_path(JsonC_INCLUDE_DIR NAMES json-c/json.h)

find_package_handle_standard_args(
    JsonC
    REQUIRED_VARS JsonC_LIBRARY JsonC_INCLUDE_DIR
)

if(JsonC_FOUND)
    mark_as_advanced(JsonC_LIBRARY)
    mark_as_advanced(JsonC_INCLUDE_DIR)
endif()

if(JsonC_FOUND AND NOT TARGET JsonC::JsonC)
    add_library(JsonC::JsonC UNKNOWN IMPORTED)
    set_target_properties(
        JsonC::JsonC PROPERTIES
        IMPORTED_LOCATION "${JsonC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${JsonC_INCLUDE_DIR}"
    )
endif()
