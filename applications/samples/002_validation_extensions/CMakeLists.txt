# Copyright (c) 2021 Leon Brands, Rythe Interactive
# SPDX-License-Identifier: MIT

project(002_validation_extensions LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(002_validation_extensions ${source})

target_compile_options(002_validation_extensions PRIVATE ${LLRI_COMPILER_FLAGS})
target_link_options(002_validation_extensions PRIVATE ${LLRI_LINKER_FLAGS})
target_compile_features(002_validation_extensions PRIVATE cxx_std_17)

include_directories(${LLRI_DIR_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(002_validation_extensions ${LLRI_SELECTED_APP_IMPLEMENTATION})

if(CMAKE_DL_LIBS)
    target_link_libraries(002_validation_extensions ${CMAKE_DL_LIBS})
endif()
