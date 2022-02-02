cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(GeodeAPI VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE CORE_SOURCES
	src/*.cpp
)

set(SOURCE_FILES 
	${CORE_SOURCES}
)
set(INCLUDE_DIRECTORIES
	"include"
	"include/keybinds"
)
set(LINK_DIRECTORIES)
set(LINK_LIBRARIES)

if (APPLE) 
	set (LINK_LIBRARIES ${LINK_LIBRARIES} "-framework Cocoa")
endif()

include(sdk/SDK/BuildFiles/cmake/Root.cmake)
set_target_properties(GeodeAPI PROPERTIES PREFIX "" OUTPUT_NAME "GeodeAPI")

geode_link_bin(GeodeAPI "${CMAKE_CURRENT_SOURCE_DIR}/bin")
