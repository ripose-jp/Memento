############# kitenlib #################
set(KITEN_LIB_VERSION "5.0.0")
set(KITEN_LIB_SOVERSION "5")

set(kitengen_SRCS
		xjdxgen.c)
set(edict_SRCS
		DictEdict/deinflection.cpp
		DictEdict/dictfileedict.cpp
		DictEdict/entryedict.cpp
		DictEdict/indexededictfile.cpp
		DictEdict/linearedictfile.cpp)
set(kanjidic_SRCS
		DictKanjidic/dictfilekanjidic.cpp
		DictKanjidic/entrykanjidic.cpp)
set(dictionary_SRCS ${edict_SRCS} ${kanjidic_SRCS} ${kitengen_SRCS})

set(kiten_LIB_SRCS
		dictionarymanager.cpp
		dictionarypreferencedialog.cpp
		dictquery.cpp
		entry.cpp
		entrylist.cpp
		historyptrlist.cpp
		${dictionary_SRCS})

add_library(kiten ${kiten_LIB_SRCS})
target_link_libraries(kiten
		Qt5::Core
		Qt5::Widgets)