add_library(
    flowlayout
    flowlayout.cpp
)
target_link_libraries(
    flowlayout
    Qt5::Widgets
)

add_library(
    strokelabel
    strokelabel.cpp
)
target_link_libraries(
    strokelabel
    Qt5::Widgets
)

add_library(
    sliderjumpstyle
    sliderjumpstyle.cpp
)
target_link_libraries(
    sliderjumpstyle
    Qt5::Widgets
)

add_library(
    scrollcombobox
    scrollcombobox.h
)
target_link_libraries(
    scrollcombobox
    Qt5::Widgets
)

add_library(
    scrolldoublespinbox
    scrolldoublespinbox.h
)
target_link_libraries(
    scrolldoublespinbox
    Qt5::Widgets
)
