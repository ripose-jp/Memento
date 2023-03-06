if(OCR_SUPPORT)
    add_library(
        ocrmodel STATIC
        ocrmodel.cpp
        ocrmodel.h
    )
    target_compile_features(ocrmodel PUBLIC cxx_std_17)
    target_compile_options(ocrmodel PRIVATE ${MEMENTO_COMPILER_FLAGS})
    target_include_directories(ocrmodel PRIVATE ${MEMENTO_INCLUDE_DIRS})
    target_link_libraries(
        ocrmodel
        PRIVATE mocr++
        PRIVATE Qt5::Concurrent
        PRIVATE Qt5::Gui
        PUBLIC Qt5::Core
    )
endif()
