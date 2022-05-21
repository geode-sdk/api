cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(GeodeAPI VERSION 0.1.0 LANGUAGES C CXX)

set(BUILD_SHARED_LIBS CACHE BOOL "enable shared library")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE CORE_SOURCES
	src/*.cpp
)
file(GLOB_RECURSE HEADERS
	include/*.hpp
)

add_library(${PROJECT_NAME} SHARED
	${CORE_SOURCES}
	src/hooks/objcHook.mm
    src/dispatch/MacMouseEvent.mm
)
target_include_directories(${PROJECT_NAME} PUBLIC
	"hash"
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

add_compile_definitions(EXPORTING_GEODE_API)

if (APPLE)
	target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
endif()

find_path(GEODE_SDK_PATH
	NAMES Geode.cmake
	PATHS $ENV{GEODE_SUITE}/sdk /usr/local/geode/sdk ~/.geode/sdk
	DOC "Geode SDK path."
	REQUIRED
)
include(${GEODE_SDK_PATH}/Geode.cmake)
setup_geode_mod(BUILD_CODEGEN OUTPUT "GeodeAPI")

if (APPLE)
	set_source_files_properties(src/hooks/objcHook.mm PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
endif()

target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADERS})

add_subdirectory(md4c)

target_link_libraries(${PROJECT_NAME} md4c)

add_subdirectory(hash)
