# Copyright 2021-2021 Leon Brands. All rights served.
# License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE

project(llri-vk LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_library(llri-vk ${source})

target_compile_features(llri-vk PRIVATE cxx_std_17)

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
