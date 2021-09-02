# CMake module for finding sqlite

# Look for the header file
if(APPLE)
    find_path(
        SQLITE_INCLUDE_DIR
        NAMES sqlite.h
    )
    find_library(
        SQLITE_LIBRARY
        NAMES sqlite
              sqlite3
              libsqlite
              libsqlite3
    )
else()
    find_path(
        SQLITE_INCLUDE_DIR
        NAMES sqlite.h
    )
    find_library(
        SQLITE_LIBRARY
        NAMES sqlite
              sqlite3
              libsqlite
              libsqlite3
    )
endif()

include(FindPackageHandleStandardArgs)
