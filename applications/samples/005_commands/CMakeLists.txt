# Copyright (c) 2021 Leon Brands, Rythe Interactive
# SPDX-License-Identifier: MIT

project(005_commands LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(005_commands ${source})

target_compile_options(005_commands PRIVATE ${LLRI_COMPILER_FLAGS})
target_link_options(005_commands PRIVATE ${LLRI_LINKER_FLAGS})
target_compile_features(005_commands PRIVATE cxx_std_17)

include_directories(${LLRI_DIR_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(005_commands ${LLRI_SELECTED_APP_IMPLEMENTATION})

if(CMAKE_DL_LIBS)
    target_link_libraries(005_commands ${CMAKE_DL_LIBS})
endif()
