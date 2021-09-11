# Copyright 2021-2021 Leon Brands. All rights served.
# License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE

project(llri LANGUAGES CXX)

file(GLOB_RECURSE source *.hpp *.inl)
add_custom_target(llri SOURCES ${source})
