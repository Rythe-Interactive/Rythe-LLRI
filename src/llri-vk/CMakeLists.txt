# Copyright (c) 2021 Leon Brands, Rythe Interactive
# SPDX-License-Identifier: MIT

project(llri-vk LANGUAGES ${LLRI_SOURCE_LANGUAGES})

file(GLOB_RECURSE source ${LLRI_SOURCE_FILETYPES})
add_library(llri-vk ${source})
add_library(llri::vk ALIAS llri-vk)

target_compile_options(llri-vk PRIVATE ${LLRI_COMPILER_FLAGS})
target_compile_features(llri-vk PRIVATE cxx_std_17)

if (APPLE)
	target_link_libraries(llri-vk PRIVATE "-framework Cocoa")
endif()

include_directories(${LLRI_DIR_SRC})
include_directories(${LLRI_DIR_DEPS}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Create /lib folder and copy build files to it
add_custom_command(TARGET llri-vk POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory 
		${LLRI_DIR_OUTPUT_LIB})

add_custom_command(TARGET llri-vk POST_BUILD
	COMMAND $<$<CONFIG:Debug>:${CMAKE_COMMAND}> -E copy
		$<TARGET_FILE:llri-vk>
		"${LLRI_DIR_OUTPUT_LIB}/llri-vk-d.lib")
	
add_custom_command(TARGET llri-vk POST_BUILD
	COMMAND $<$<CONFIG:Release>:${CMAKE_COMMAND}> -E copy
		$<TARGET_FILE:llri-vk>
		"${LLRI_DIR_OUTPUT_LIB}/llri-vk.lib")

# Create /include folder and copy llri headers to it
add_custom_command(TARGET llri-vk PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory 
		${LLRI_DIR_OUTPUT_INCLUDE})

add_custom_command(TARGET llri-vk PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory 
		${LLRI_DIR_OUTPUT_INCLUDE}/llri)

add_custom_command(TARGET llri-vk PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory 
		${LLRI_DIR_OUTPUT_INCLUDE}/llri)

add_custom_command(TARGET llri-vk PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory 
		${LLRI_DIR_OUTPUT_INCLUDE}/llri/detail)

add_custom_command(TARGET llri-vk PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		${LLRI_DIR_SRC}/llri/llri.hpp
		${LLRI_DIR_OUTPUT_INCLUDE}/llri)

file(GLOB detailHeaders ${LLRI_DIR_SRC}/llri/detail/*.hpp ${LLRI_DIR_SRC}/llri/detail/*.inl)
add_custom_command(TARGET llri-vk PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		${detailHeaders}
		${LLRI_DIR_OUTPUT_INCLUDE}/llri/detail)
