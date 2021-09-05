cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

# Folder structure
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY ON)
set(CMAKE_FOLDER CMakePredefinedTargets)

# Project
project(llri LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include threading
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(legion/engine/llri)
set_property(TARGET llri PROPERTY FOLDER "llri")

if(WIN32)
	add_subdirectory(legion/engine/llri-dx)
	set_property(TARGET llri-dx PROPERTY FOLDER "llri")

	add_subdirectory(legion/engine/llri-vk)
	set_property(TARGET llri-vk PROPERTY FOLDER "llri")
	
	set(LLRI_IMPLEMENTATION_OPTIONS "llri-dx;llri-vk")
	set(LLRI_SELECTED_APP_IMPLEMENTATION "llri-dx" CACHE STRING "Implementation build for samples/unit tests chosen by the user at CMake configure time.")
	set_property(CACHE LLRI_SELECTED_APP_IMPLEMENTATION PROPERTY STRINGS ${LLRI_IMPLEMENTATION_OPTIONS})

elseif(APPLE)
	# TODO: apple might also support vulkan builds through moltenvk
	add_subdirectory(legion/engine/llri-mtl)
	set_property(TARGET llri-mtl PROPERTY FOLDER "llri")

	set(LLRI_IMPLEMENTATION_OPTIONS "llri-mtl")
	set(LLRI_SELECTED_APP_IMPLEMENTATION "llri-mtl" CACHE STRING "Implementation build for samples/unit tests chosen by the user at CMake configure time.")
	set_property(CACHE LLRI_SELECTED_APP_IMPLEMENTATION PROPERTY STRINGS ${LLRI_IMPLEMENTATION_OPTIONS})

elseif(UNIX) # elseif here guarantees that unix is not apple unix
	add_subdirectory(legion/engine/llri-vk)
	set_property(TARGET llri-vk PROPERTY FOLDER "llri")

	set(LLRI_IMPLEMENTATION_OPTIONS "llri-vk")
	set(LLRI_SELECTED_APP_IMPLEMENTATION "llri-vk" CACHE STRING "Implementation build for samples/unit tests chosen by the user at CMake configure time.")
	set_property(CACHE LLRI_SELECTED_APP_IMPLEMENTATION PROPERTY STRINGS ${LLRI_IMPLEMENTATION_OPTIONS})

endif()

add_subdirectory(applications)