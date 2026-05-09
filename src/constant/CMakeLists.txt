configure_file(
    "${PROJECT_SOURCE_DIR}/src/constant/version.h.in"
    "${PROJECT_BINARY_DIR}/src/constant/version.h"
)
add_library(version INTERFACE "${PROJECT_BINARY_DIR}/src/constant/version.h")
target_compile_features(version INTERFACE cxx_std_20)
target_compile_options(version INTERFACE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(version INTERFACE "${PROJECT_BINARY_DIR}/src")

add_library(constants INTERFACE constants.h)
target_compile_features(constants INTERFACE cxx_std_20)
target_compile_options(constants INTERFACE ${MEMENTO_COMPILER_FLAGS})
