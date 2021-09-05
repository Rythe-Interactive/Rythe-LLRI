cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(llri LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source *.hpp *.inl)

add_custom_target(llri SOURCES ${source})
