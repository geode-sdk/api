cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(GeodeAPI VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE CORE_SOURCES
	src/*.cpp
)

set(GEODE_BUILD_CODEGEN On)

set(SOURCE_FILES 
	${CORE_SOURCES}
	src/hooks/objcHook.mm
)
set(INCLUDE_DIRECTORIES
	"include"
	"include/keybinds"
	"include/dragdrop"
	"src"
	"src/gui/loading"
	"src/gui"
	"src/gui/mods"
	"src/gui/mods/dev"
	"src/gui/mods/info"
	"src/gui/mods/list"
	"src/gui/mods/settings"
)
set(LINK_DIRECTORIES)
set(LINK_LIBRARIES)
set(BINARY_NAME GeodeAPI)

add_compile_definitions(EXPORTING_GEODE_API)

if (APPLE) 
	set (LINK_LIBRARIES ${LINK_LIBRARIES} "-framework Cocoa")
endif()

include(sdk/Geode.cmake)
