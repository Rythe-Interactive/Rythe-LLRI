cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(002_validation_ext LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(002_validation_ext ${source})

include_directories(${LLRI_DIR_SRC})
include_directories(${PROJECT_SOURCE_DIR})

target_link_libraries(002_validation_ext ${LLRI_SELECTED_APP_IMPLEMENTATION})
