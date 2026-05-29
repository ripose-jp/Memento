add_library(
    ocr
    ocrcontroller.cpp
    ocrcontroller.h
)
if(MEMENTO_OCR_SUPPORT)
    target_sources(
        ocr
        PRIVATE ocrmodel.cpp
        PRIVATE ocrmodel.h
    )
endif()
target_compile_features(ocr PUBLIC cxx_std_20)
target_compile_options(ocr PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(ocr PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    ocr
    PRIVATE "$<$<BOOL:${MEMENTO_OCR_SUPPORT}>:$<$<BOOL:${MEMENTO_SYSTEM_MOCR}>:mocr::mocrxx>>"
    PRIVATE "$<$<BOOL:${MEMENTO_OCR_SUPPORT}>:$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_MOCR}>>:mocr++>>"
    PRIVATE mpvplayer
    PRIVATE Qt6::Concurrent
    PRIVATE Qt6::Gui
    PRIVATE settings
    PUBLIC "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Coro>"
    PUBLIC "$<$<BOOL:${MEMENTO_SYSTEM_QCORO}>:QCoro::Qml>"
    PUBLIC "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Coro>"
    PUBLIC "$<$<NOT:$<BOOL:${MEMENTO_SYSTEM_QCORO}>>:QCoro6Qml>"
    PUBLIC Qt6::Core
)
