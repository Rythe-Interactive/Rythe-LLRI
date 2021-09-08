# Copyright 2021-2021 Leon Brands. All rights served.
# License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(unit_tests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(unit_tests ${source})

include_directories(${LLRI_DIR_APPLICATIONS_DEPS}/include/)
include_directories(${LLRI_DIR_SRC})
include_directories(${PROJECT_SOURCE_DIR})

target_link_libraries(unit_tests ${LLRI_SELECTED_APP_IMPLEMENTATION})
