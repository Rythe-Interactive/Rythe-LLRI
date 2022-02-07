# Copyright (c) 2021 Leon Brands
# SPDX-License-Identifier: MIT

project(001_validation LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(001_validation ${source})

target_compile_options(001_validation PRIVATE ${LLRI_COMPILER_FLAGS})
target_compile_features(001_validation PRIVATE cxx_std_17)

include_directories(${LLRI_DIR_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(001_validation ${LLRI_SELECTED_APP_IMPLEMENTATION})

if(CMAKE_DL_LIBS)
    target_link_libraries(001_validation ${CMAKE_DL_LIBS})
endif()
