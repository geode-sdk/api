cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(GeodeAPI VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE CORE_SOURCES
	src/*.cpp
)

# set(GEODE_BUILD_CODEGEN On)
set(GEODE_AUTO_INSTALL_IN_API On)

set(SOURCE_FILES 
	${CORE_SOURCES}
)
set(INCLUDE_DIRECTORIES
	"include"
	"include/keybinds"
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

add_compile_definitions(EXPORTING_GEODE_API)

if (APPLE) 
	set (LINK_LIBRARIES ${LINK_LIBRARIES} "-framework Cocoa")
endif()

include(sdk/SDK/BuildFiles/cmake/Root.cmake)
set_target_properties(GeodeAPI PROPERTIES PREFIX "" OUTPUT_NAME "GeodeAPI")

geode_link_bin(GeodeAPI "${CMAKE_CURRENT_SOURCE_DIR}/bin")
