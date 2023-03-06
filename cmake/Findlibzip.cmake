include(FindPackageHandleStandardArgs)

find_library(libzip_LIBRARY NAMES zip libzip)
find_path(libzip_INCLUDE_DIR NAMES zip.h)

find_package_handle_standard_args(
    libzip
    REQUIRED_VARS libzip_LIBRARY libzip_INCLUDE_DIR
)

if(libzip_FOUND)
    mark_as_advanced(libzip_LIBRARY)
    mark_as_advanced(libzip_INCLUDE_DIR)
endif()

if(libzip_FOUND AND NOT TARGET libzip::libzip)
add_library(libzip::libzip UNKNOWN IMPORTED)
set_target_properties(
    libzip::libzip PROPERTIES
    IMPORTED_LOCATION "${libzip_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${libzip_INCLUDE_DIR}"
)
endif()
