# Copyright 2021-2021 Leon Brands. All rights served.
# License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE

add_subdirectory(unit_tests)
add_subdirectory(sandbox)

set_target_properties(
	unit_tests 
	sandbox 
	PROPERTIES FOLDER "applications"
)

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
