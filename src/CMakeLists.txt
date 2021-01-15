add_subdirectory(util)
add_subdirectory(dict)
add_subdirectory(anki)
add_subdirectory(gui)

if(WIN32)
	set(APP_ICON_RESOURCE_WINDOWS ../res/appicon.rc)
endif()

add_executable(
    memento 
    main.cpp 
    ../res/resources.qrc
    ../config/input.conf
    ../config/dict/JMDict
    ${APP_ICON_RESOURCE_WINDOWS}
)
target_link_libraries(memento mainwindow)