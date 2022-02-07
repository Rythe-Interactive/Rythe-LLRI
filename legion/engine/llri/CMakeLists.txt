# Copyright (c) 2021 Leon Brands, Rythe Interactive
# SPDX-License-Identifier: MIT

project(llri LANGUAGES CXX)

file(GLOB_RECURSE source *.h *.hpp *.inl)
add_custom_target(llri SOURCES ${source})
