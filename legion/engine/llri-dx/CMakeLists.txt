cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(llri-dx LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source *.hpp *.cpp)
add_library(llri-dx ${source})

include_directories(../)
include_directories(../../../deps/include)
include_directories(${PROJECT_SOURCE_DIR})