# Copyright (c) 2021 Leon Brands, Rythe Interactive
# SPDX-License-Identifier: MIT

project(sandbox LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(sandbox ${source})

target_compile_options(sandbox PRIVATE ${LLRI_COMPILER_FLAGS})
target_link_options(sandbox PRIVATE ${LLRI_LINKER_FLAGS})
target_compile_features(sandbox PRIVATE cxx_std_17)

include_directories(${LLRI_DIR_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sandbox ${LLRI_SELECTED_APP_IMPLEMENTATION})

if(CMAKE_DL_LIBS)
    target_link_libraries(sandbox ${CMAKE_DL_LIBS})
endif()

if (UNIX AND NOT APPLE)
	target_link_libraries(llri-vk PRIVATE X11-xcb)
endif()
