# Copyright (c) 2021 Leon Brands, Rythe Interactive
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# General CMake config
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY ON)
set(CMAKE_FOLDER CMakePredefinedTargets)

# Project
project(llri LANGUAGES CXX)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Language
if (APPLE)
	set(LLRI_SOURCE_LANGUAGES OBJCXX)
	set(LLRI_SOURCE_FILETYPES *.hpp *.h *.cpp *.c *.inl *.mm *.m)
else()
	set(LLRI_SOURCE_LANGUAGES CXX)
	set(LLRI_SOURCE_FILETYPES *.hpp *.h *.cpp *.c *.inl)
endif()

# Easy folder references for subdirs
set(LLRI_DIR_OUTPUT_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE INTERNAL "")
set(LLRI_DIR_OUTPUT_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib" CACHE INTERNAL "")

set(LLRI_DIR_DEPS "${CMAKE_CURRENT_SOURCE_DIR}/deps" CACHE INTERNAL "")
set(LLRI_DIR_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src" CACHE INTERNAL "")

set(LLRI_DIR_APPLICATIONS "${CMAKE_CURRENT_SOURCE_DIR}/applications" CACHE INTERNAL "")
set(LLRI_DIR_APPLICATIONS_DEPS "${CMAKE_CURRENT_SOURCE_DIR}/applications/deps" CACHE INTERNAL "")

if (NOT DEFINED LLRI_ENABLE_ASAN)
 	SET(LLRI_ENABLE_ASAN OFF CACHE BOOL "Enable address sanitizer.")
 endif()

if (NOT MSVC)
	set(LLRI_COMPILER_FLAGS -Wall -Wextra -Wpedantic -Werror 
							-Wno-c++98-compat -Wno-c++98-compat-pedantic 
							-Wno-c++0x-compat 
							-Wno-c++11-compat -Wno-c++11-compat-pedantic
							-Wno-c++14-compat -Wno-c++14-compat-pedantic)
	set(LLRI_LINKER_FLAGS "")

	if (${LLRI_ENABLE_ASAN})
		set(LLRI_COMPILER_FLAGS -fsanitize=address ${LLRI_COMPILER_FLAGS})
		set(LLRI_LINKER_FLAGS -fsanitize=address ${LLRI_LINKER_FLAGS})
	endif()
else()
	set(LLRI_COMPILER_FLAGS /W4 /WX)
	set(LLRI_LINKER_FLAGS "")

	if ($LLRI_ENABLE_ASAN})
		set(LLRI_COMPILER_FLAGS /fsanitize=address ${LLRI_COMPILER_FLAGS})
	endif()
endif()

# Add LLRI headers & source
add_subdirectory(${LLRI_DIR_SRC}/llri)
set_property(TARGET llri PROPERTY FOLDER "llri")

if(WIN32)
	add_subdirectory(${LLRI_DIR_SRC}/llri-dx)
	add_subdirectory(${LLRI_DIR_SRC}/llri-vk)
	set_target_properties(llri-dx llri-vk PROPERTIES FOLDER "llri")
	
	set(LLRI_IMPLEMENTATION_OPTIONS "llri-dx;llri-vk")
	set(LLRI_SELECTED_APP_IMPLEMENTATION "llri-dx" CACHE STRING "Implementation build for applications, selected by the user at CMake configure time.")
	set_property(CACHE LLRI_SELECTED_APP_IMPLEMENTATION PROPERTY STRINGS ${LLRI_IMPLEMENTATION_OPTIONS})

elseif(APPLE)
	add_subdirectory(${LLRI_DIR_SRC}/llri-vk)
	set_property(TARGET llri-vk PROPERTY FOLDER "llri")

	set(LLRI_IMPLEMENTATION_OPTIONS "llri-vk")
	set(LLRI_SELECTED_APP_IMPLEMENTATION "llri-vk" CACHE STRING "Implementation build for applications, selected by the user at CMake configure time.")
	set_property(CACHE LLRI_SELECTED_APP_IMPLEMENTATION PROPERTY STRINGS ${LLRI_IMPLEMENTATION_OPTIONS})

elseif(UNIX) # elseif here guarantees that unix is not apple unix
	add_subdirectory(${LLRI_DIR_SRC}/llri-vk)
	set_property(TARGET llri-vk PROPERTY FOLDER "llri")

	set(LLRI_IMPLEMENTATION_OPTIONS "llri-vk")
	set(LLRI_SELECTED_APP_IMPLEMENTATION "llri-vk" CACHE STRING "Implementation build for applications, selected by the user at CMake configure time.")
	set_property(CACHE LLRI_SELECTED_APP_IMPLEMENTATION PROPERTY STRINGS ${LLRI_IMPLEMENTATION_OPTIONS})

endif()

# Applications
if (NOT DEFINED LLRI_BUILD_TESTS)
	SET(LLRI_BUILD_TESTS ON CACHE BOOL "Add unit tests to the project. Requires the submodules in applications/deps/ to be present.")
endif()

if (NOT DEFINED LLRI_SELECTED_TEST_ADAPTERS)
	set(LLRI_SELECTED_TEST_ADAPTERS "All" CACHE STRING 
		"List of adapters used in the unit tests. Elements are case sensitive and must be separated by a semi-colon (;).
		If set to \"All\", all compatible adapters are used for the same unit test session. In this case at least one compatible adapter must be available for the test to succeed.
		If set to one or more adapter names, each selected adapter must be available for the test to succeed.")
endif()

if (NOT DEFINED LLRI_BUILD_APPLICATIONS)
	SET(LLRI_BUILD_APPLICATIONS ON CACHE BOOL "Add applications (samples, sandbox) to the project. Requires the submodules in applications/deps/ to be present.")
endif()

if (${LLRI_BUILD_APPLICATIONS} OR ${LLRI_BUILD_TESTS})
	add_subdirectory(applications)
endif()
