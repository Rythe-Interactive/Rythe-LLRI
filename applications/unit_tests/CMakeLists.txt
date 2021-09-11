# Copyright 2021-2021 Leon Brands. All rights served.
# License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE

project(unit_tests LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(unit_tests ${source})

target_compile_features(unit_tests PRIVATE cxx_std_17)

include_directories(${LLRI_DIR_APPLICATIONS_DEPS}/include/)
include_directories(${LLRI_DIR_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(unit_tests ${LLRI_SELECTED_APP_IMPLEMENTATION})
