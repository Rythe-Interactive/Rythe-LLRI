# Copyright (c) 2021 Leon Brands, Rythe Interactive
# SPDX-License-Identifier: MIT

project(003_adapter_selection LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(003_adapter_selection ${source})

target_compile_options(003_adapter_selection PRIVATE ${LLRI_COMPILER_FLAGS})
target_link_options(003_adapter_selection PRIVATE ${LLRI_LINKER_FLAGS})
target_compile_features(003_adapter_selection PRIVATE cxx_std_17)

include_directories(${LLRI_DIR_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(003_adapter_selection ${LLRI_SELECTED_APP_IMPLEMENTATION})

if(CMAKE_DL_LIBS)
    target_link_libraries(003_adapter_selection ${CMAKE_DL_LIBS})
endif()
