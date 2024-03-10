add_library(
    flowlayout STATIC
    flowlayout.cpp
    flowlayout.h
)
target_compile_features(flowlayout PUBLIC cxx_std_17)
target_compile_options(flowlayout PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(flowlayout PUBLIC Qt6::Widgets)

add_library(
    hittestwidget STATIC
    hittestwidget.cpp
    hittestwidget.h
)
target_compile_features(hittestwidget PUBLIC cxx_std_17)
target_compile_options(hittestwidget PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(hittestwidget PUBLIC Qt6::Widgets)

add_library(
    strokelabel STATIC
    strokelabel.cpp
    strokelabel.h
)
target_compile_features(strokelabel PUBLIC cxx_std_17)
target_compile_options(strokelabel PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(strokelabel PUBLIC Qt6::Widgets)

add_library(
    sliderjumpstyle STATIC
    sliderjumpstyle.h
)
target_compile_features(sliderjumpstyle PUBLIC cxx_std_17)
target_compile_options(sliderjumpstyle PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(sliderjumpstyle PUBLIC Qt6::Widgets)

add_library(
    scrollcombobox STATIC
    scrollcombobox.h
)
target_compile_features(scrollcombobox PUBLIC cxx_std_17)
target_compile_options(scrollcombobox PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(scrollcombobox PUBLIC Qt6::Widgets)

add_library(
    scrolldoublespinbox STATIC
    scrolldoublespinbox.h
)
target_compile_features(scrolldoublespinbox PUBLIC cxx_std_17)
target_compile_options(scrolldoublespinbox PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_link_libraries(scrolldoublespinbox PUBLIC Qt6::Widgets)
