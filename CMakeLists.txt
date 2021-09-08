# Copyright 2021-2021 Leon Brands. All rights served.
# License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

# Folder structure
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY ON)
set(CMAKE_FOLDER CMakePredefinedTargets)

# Project
project(llri LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Easy folder references for subdirs
set(LLRI_DIR_OUTPUT_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LLRI_DIR_OUTPUT_LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(LLRI_DIR_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/deps)
set(LLRI_DIR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/legion/engine)

set(LLRI_DIR_APPLICATIONS ${CMAKE_CURRENT_SOURCE_DIR}/applications)
set(LLRI_DIR_APPLICATIONS_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/applications/deps)

# Include threading
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(${LLRI_DIR_SRC}/llri)
set_property(TARGET llri PROPERTY FOLDER "llri")

if(WIN32)
	add_subdirectory(${LLRI_DIR_SRC}/llri-dx)
	set_property(TARGET llri-dx PROPERTY FOLDER "llri")

	add_subdirectory(${LLRI_DIR_SRC}/llri-vk)
	set_property(TARGET llri-vk PROPERTY FOLDER "llri")
	
	set(LLRI_IMPLEMENTATION_OPTIONS "llri-dx;llri-vk")
	set(LLRI_SELECTED_APP_IMPLEMENTATION "llri-dx" CACHE STRING "Implementation build for samples/unit tests chosen by the user at CMake configure time.")
	set_property(CACHE LLRI_SELECTED_APP_IMPLEMENTATION PROPERTY STRINGS ${LLRI_IMPLEMENTATION_OPTIONS})

elseif(APPLE)
	# TODO: apple might also support vulkan builds through moltenvk
	add_subdirectory(${LLRI_DIR_SRC}/llri-mtl)
	set_property(TARGET llri-mtl PROPERTY FOLDER "llri")

	set(LLRI_IMPLEMENTATION_OPTIONS "llri-mtl")
	set(LLRI_SELECTED_APP_IMPLEMENTATION "llri-mtl" CACHE STRING "Implementation build for samples/unit tests chosen by the user at CMake configure time.")
	set_property(CACHE LLRI_SELECTED_APP_IMPLEMENTATION PROPERTY STRINGS ${LLRI_IMPLEMENTATION_OPTIONS})

elseif(UNIX) # elseif here guarantees that unix is not apple unix
	add_subdirectory(${LLRI_DIR_SRC}/llri-vk)
	set_property(TARGET llri-vk PROPERTY FOLDER "llri")

	set(LLRI_IMPLEMENTATION_OPTIONS "llri-vk")
	set(LLRI_SELECTED_APP_IMPLEMENTATION "llri-vk" CACHE STRING "Implementation build for samples/unit tests chosen by the user at CMake configure time.")
	set_property(CACHE LLRI_SELECTED_APP_IMPLEMENTATION PROPERTY STRINGS ${LLRI_IMPLEMENTATION_OPTIONS})

endif()

add_subdirectory(applications)