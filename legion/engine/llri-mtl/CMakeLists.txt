cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(llri-mtl LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-x objective-c++")

file(GLOB_RECURSE source *.hpp *.inl *.cpp *.mm)
add_library(llri-mtl ${source})

include_directories(${LLRI_DIR_SRC})
include_directories(${LLRI_DIR_DEPS}/include)
include_directories(${PROJECT_SOURCE_DIR})

# Create /lib folder and copy build files to it
add_custom_command(TARGET llri-mtl POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory 
		${LLRI_DIR_OUTPUT_LIB})

add_custom_command(TARGET llri-mtl POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		"Debug/llri-mtl.a"
		"${LLRI_DIR_OUTPUT_LIB}/llri-mtl-d.a")

add_custom_command(TARGET llri-mtl POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		"Release/llri-mtl.a"
		"${LLRI_DIR_OUTPUT_LIB}/llri-mtl.a")

# Create /include folder and copy llri headers to it
add_custom_command(TARGET llri-mtl PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory 
		${LLRI_DIR_OUTPUT_INCLUDE})

add_custom_command(TARGET llri-mtl PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory 
		${LLRI_DIR_OUTPUT_INCLUDE}/llri)

add_custom_command(TARGET llri-mtl PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory 
		${LLRI_DIR_OUTPUT_INCLUDE}/llri)

add_custom_command(TARGET llri-mtl PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory 
		${LLRI_DIR_OUTPUT_INCLUDE}/llri/detail)

add_custom_command(TARGET llri-mtl PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		${LLRI_DIR_SRC}/llri/llri.hpp
		${LLRI_DIR_OUTPUT_INCLUDE}/llri)

file(GLOB detailHeaders ${LLRI_DIR_SRC}/llri/detail/*.hpp ${LLRI_DIR_SRC}/llri/detail/*.inl)
add_custom_command(TARGET llri-mtl PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		${detailHeaders}
		${LLRI_DIR_OUTPUT_INCLUDE}/llri/detail)
