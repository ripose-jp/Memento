add_library(
    flowlayout
    flowlayout.cpp
)
target_link_libraries(
    flowlayout
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