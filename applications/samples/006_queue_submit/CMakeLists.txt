# Copyright 2021-2021 Leon Brands. All rights served.
# License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE

project(006_queue_submit LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(006_queue_submit ${source})

target_compile_options(006_queue_submit PRIVATE ${LLRI_COMPILER_FLAGS})
target_compile_features(006_queue_submit PRIVATE cxx_std_17)

include_directories(${LLRI_DIR_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(006_queue_submit ${LLRI_SELECTED_APP_IMPLEMENTATION})

if(CMAKE_DL_LIBS)
    target_link_libraries(006_queue_submit ${CMAKE_DL_LIBS})
endif()
