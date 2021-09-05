cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(004_device LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source *.hpp *.cpp)
add_executable(004_device ${source})

include_directories(../../legion/engine)
include_directories(${PROJECT_SOURCE_DIR})

target_link_libraries(004_device ${LLRI_SELECTED_APP_IMPLEMENTATION})