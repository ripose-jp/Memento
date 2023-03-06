include(FindPackageHandleStandardArgs)

find_library(MeCab_LIBRARY NAMES mecab)
find_path(MeCab_INCLUDE_DIR NAMES mecab.h)

find_package_handle_standard_args(
    MeCab
    REQUIRED_VARS MeCab_LIBRARY MeCab_INCLUDE_DIR
)

if(MeCab_FOUND)
    mark_as_advanced(MeCab_LIBRARY)
    mark_as_advanced(MeCab_INCLUDE_DIR)
endif()

if(MeCab_FOUND AND NOT TARGET MeCab::MeCab)
    add_library(MeCab::MeCab UNKNOWN IMPORTED)
    set_target_properties(
        MeCab::MeCab PROPERTIES
        IMPORTED_LOCATION "${MeCab_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${MeCab_INCLUDE_DIR}"
    )
endif()
