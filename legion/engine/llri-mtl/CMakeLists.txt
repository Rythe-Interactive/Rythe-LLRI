cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(llri-mtl LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-x objective-c++")

file(GLOB_RECURSE source *.hpp *.inl *.cpp *.mm)
add_library(llri-mtl ${source})
