add_subdirectory(unit_tests)
set_property(TARGET unit_tests PROPERTY FOLDER "applications")

add_subdirectory(sandbox)
set_property(TARGET sandbox PROPERTY FOLDER "applications")

add_subdirectory(samples/000_hello_llri)
set_property(TARGET 000_hello_llri PROPERTY FOLDER "applications/samples")

add_subdirectory(samples/001_validation)
set_property(TARGET 001_validation PROPERTY FOLDER "applications/samples")

add_subdirectory(samples/002_validation_ext)
set_property(TARGET 002_validation_ext PROPERTY FOLDER "applications/samples")

add_subdirectory(samples/003_adapter_selection)
set_property(TARGET 003_adapter_selection PROPERTY FOLDER "applications/samples")

add_subdirectory(samples/004_device)
set_property(TARGET 004_device PROPERTY FOLDER "applications/samples")
