# Copyright 2021-2021 Leon Brands. All rights served.
# License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE

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
add_subdirectory(deps/doctest)
set_target_properties(
	doctest
	PROPERTIES FOLDER "applications/deps"
)

# add applications
add_subdirectory(unit_tests)
set_target_properties(unit_tests PROPERTIES FOLDER "applications")
target_include_directories(unit_tests PUBLIC deps/doctest)

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
