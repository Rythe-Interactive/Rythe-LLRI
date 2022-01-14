# Copyright 2021-2021 Leon Brands. All rights served.
# License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE

project(sandbox LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(sandbox ${source})

target_compile_features(sandbox PRIVATE cxx_std_17)

include_directories(${LLRI_DIR_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sandbox ${LLRI_SELECTED_APP_IMPLEMENTATION})

if(CMAKE_DL_LIBS)
    target_link_libraries(sandbox ${CMAKE_DL_LIBS})
endif()
