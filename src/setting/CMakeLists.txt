add_library(
    settings
    audiosource.h
    audiosourcemodel.cpp
    audiosourcemodel.h
    keybindprofile.cpp
    keybindprofile.h
    keybindprofilemodel.cpp
    keybindprofilemodel.h
    keys.h
    settings.cpp
    settings.h
)
target_compile_features(settings PUBLIC cxx_std_20)
target_compile_options(settings PRIVATE ${MEMENTO_COMPILER_FLAGS})
target_include_directories(settings PRIVATE ${MEMENTO_INCLUDE_DIRS})
target_link_libraries(
    settings
    PRIVATE "$<$<BOOL:${WIN32}>:utils>"
    PUBLIC Qt6::Core
    PUBLIC Qt6::Gui
    PUBLIC Qt6::Qml
)
