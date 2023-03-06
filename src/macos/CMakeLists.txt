if (APPLE)
    add_library(
        cocoa_event_handler STATIC
        cocoaeventhandler.h
        cocoaeventhandler.mm
    )
    target_compile_features(cocoa_event_handler PUBLIC cxx_std_17)
    target_compile_options(cocoa_event_handler PRIVATE ${MEMENTO_COMPILER_FLAGS})
    target_include_directories(
        cocoa_event_handler PRIVATE ${MEMENTO_INCLUDE_DIRS}
    )
    target_link_libraries(
        cocoa_event_handler
        PRIVATE "-framework cocoa"
    )
endif()
