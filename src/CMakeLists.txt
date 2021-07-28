add_subdirectory(util)
add_subdirectory(audio)
add_subdirectory(dict)
add_subdirectory(gui)
add_subdirectory(anki)

if(WIN32)
	set(APP_ICON_RESOURCE_WINDOWS ../res/appicon.rc)
    set(WINDOWS_EXE_OPTION WIN32)
endif()

add_executable(
    memento 
    ${WINDOWS_EXE_OPTION}
    main.cpp 
    ../res/resources.qrc
    ${APP_ICON_RESOURCE_WINDOWS}
)
target_link_libraries(memento mainwindow)