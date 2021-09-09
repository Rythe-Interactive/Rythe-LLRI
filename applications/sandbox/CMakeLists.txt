# Copyright 2021-2021 Leon Brands. All rights served.
# License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE

project(sandbox LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl *.cpp)
add_executable(sandbox ${source})

target_compile_features(sandbox PRIVATE cxx_std_17)

file(GLOB dllDeps ${LLRI_DIR_APPLICATIONS_DEPS}/dll/*.dll)

add_custom_command(TARGET sandbox PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${dllDeps} ${CMAKE_CURRENT_BINARY_DIR}/Debug
)

add_custom_command(TARGET sandbox PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${dllDeps} ${CMAKE_CURRENT_BINARY_DIR}/Release
)

include_directories(${LLRI_DIR_APPLICATIONS_DEPS}/include/)
include_directories(${LLRI_DIR_APPLICATIONS_DEPS}/include/legion/engine)
include_directories(${LLRI_DIR_SRC})
include_directories(${PROJECT_SOURCE_DIR})

target_link_directories(sandbox PUBLIC ${LLRI_DIR_APPLICATIONS_DEPS}/lib)

target_link_libraries(sandbox ${LLRI_SELECTED_APP_IMPLEMENTATION})

target_link_libraries(sandbox OptickCore)
target_link_libraries(sandbox OpenCL)
target_link_libraries(sandbox debug legion-core-d)
target_link_libraries(sandbox optimized legion-core)
