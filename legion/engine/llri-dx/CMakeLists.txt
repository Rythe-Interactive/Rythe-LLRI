cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(llri-dx LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_library(llri-dx ${source})

include_directories(${LLRI_DIR_SRC})
include_directories(${LLRI_DIR_DEPS}/include)
include_directories(${PROJECT_SOURCE_DIR})

# Create /lib folder and copy build files to it
add_custom_command(TARGET llri-dx POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory 
		"../../../lib")

add_custom_command(TARGET llri-dx POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		"Debug/llri-dx.lib"
		"../../../lib/llri-dx-d.lib")

add_custom_command(TARGET llri-dx POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		"Release/llri-dx.lib"
		"../../../lib/llri-dx.lib")
