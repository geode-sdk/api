cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(GeodeAPI VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE CORE_SOURCES
	src/*.cpp
)
file(GLOB_RECURSE HEADERS
	include/*.hpp
)

set(GEODE_BUILD_CODEGEN On)

set(SOURCE_FILES 
	${CORE_SOURCES}
	src/hooks/objcHook.mm
    src/dispatch/MacMouseEvent.mm
)
set(INCLUDE_DIRECTORIES
	"checksum"
	"include"
	"include/dragdrop"
	"include/nodes"
	"include/settings"
	"include/shortcuts"
	"include/views"
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
set_source_files_properties(src/hooks/objcHook.mm PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

set_source_files_properties(src/hooks/objcHook.mm PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADERS})

add_subdirectory(md4c)

target_link_libraries(${PROJECT_NAME} md4c)

add_subdirectory(checksum)
