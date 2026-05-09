add_library(
    definition
    structuredcontent.cpp
    structuredcontent.h
)
target_compile_features(definition PRIVATE cxx_std_20)
target_include_directories(definition PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_compile_options(definition PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(
    definition
    PRIVATE utils
    PUBLIC Qt6::Core
    PUBLIC settings
)