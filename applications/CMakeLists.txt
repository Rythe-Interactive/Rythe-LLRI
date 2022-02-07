# Copyright (c) 2021 Leon Brands, Rythe Interactive
# SPDX-License-Identifier: MIT

# update git submodules
if( NOT EXISTS "${LLRI_DIR_APPLICATIONS_DEPS}/glfw/CMakeLists.txt" OR
	NOT EXISTS "${LLRI_DIR_APPLICATIONS_DEPS}/doctest/CMakeLists.txt" OR
	NOT EXISTS "${LLRI_DIR_APPLICATIONS_DEPS}/stb/README.md")

	message(STATUS "The git submodules were not updated, attempting to do so now")
	find_package(Git)
	if (NOT GIT_FOUND)
		message(FATAL_ERROR "Couldn't find Git executable, meaning the git submodules weren't updated. Install git into PATH and try again, or alternatively you may disable LLRI_BUILD_APPLICATIONS.")
	else()
		message(STATUS "Git found, updating submodules")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
						RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update failed with ${GIT_SUBMOD_RESULT}")
		else()
			message(STATUS "Successfully updated submodules")
		endif()
	endif()
endif()

# add dependencies
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/glfw)
set_target_properties(
	glfw
	uninstall
	update_mappings
	PROPERTIES FOLDER "applications/deps/glfw3"
)

SET(DOCTEST_WITH_TESTS OFF CACHE BOOL "" FORCE)
SET(DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/doctest)

# add applications
add_subdirectory(unit_tests)
set_target_properties(unit_tests PROPERTIES FOLDER "applications")
target_include_directories(unit_tests PUBLIC deps/doctest)

target_include_directories(unit_tests PUBLIC deps/glfw/include)
target_link_libraries(unit_tests glfw ${GLFW_LIBRARIES})

add_subdirectory(sandbox)
set_target_properties(sandbox PROPERTIES FOLDER "applications")
target_include_directories(sandbox PUBLIC deps/glfw/include)
target_link_libraries(sandbox glfw ${GLFW_LIBRARIES})

add_subdirectory(samples/000_hello_llri)
add_subdirectory(samples/001_validation)
add_subdirectory(samples/002_validation_extensions)
add_subdirectory(samples/003_adapter_selection)
add_subdirectory(samples/004_device)
add_subdirectory(samples/005_commands)
add_subdirectory(samples/006_queue_submit)

set_target_properties(
	000_hello_llri
	001_validation
	002_validation_extensions
	003_adapter_selection
	004_device
	005_commands
	006_queue_submit
	PROPERTIES FOLDER "applications/samples"
)
