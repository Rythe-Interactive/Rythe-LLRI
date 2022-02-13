# Copyright (c) 2021 Leon Brands, Rythe Interactive
# SPDX-License-Identifier: MIT

project(unit_tests LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(unit_tests ${source})

target_compile_options(unit_tests PRIVATE ${LLRI_COMPILER_FLAGS})
target_link_options(unit_tests PRIVATE ${LLRI_LINKER_FLAGS})
target_compile_features(unit_tests PRIVATE cxx_std_17)

include_directories(${LLRI_DIR_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(unit_tests ${LLRI_SELECTED_APP_IMPLEMENTATION})

if(CMAKE_DL_LIBS)
    target_link_libraries(unit_tests ${CMAKE_DL_LIBS})
endif()

# declare unit tests as runnable tests to cmake
include(CTest)
enable_testing()

# doctest has a builtin module to detect & add tests
include(${LLRI_DIR_APPLICATIONS_DEPS}/doctest/scripts/cmake/doctest.cmake)
doctest_discover_tests(unit_tests)
