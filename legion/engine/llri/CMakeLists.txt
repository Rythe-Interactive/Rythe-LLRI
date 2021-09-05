cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(llri LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source *.hpp)

add_custom_target(llri SOURCES ${source})