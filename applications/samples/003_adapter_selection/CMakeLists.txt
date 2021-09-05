cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(003_adapter_selection LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE source *.hpp *.cpp)
add_executable(003_adapter_selection ${source})

include_directories(../../legion/engine)
include_directories(${PROJECT_SOURCE_DIR})

target_link_libraries(003_adapter_selection ${LLRI_SELECTED_APP_IMPLEMENTATION})