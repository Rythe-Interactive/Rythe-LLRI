/**
 * @file llri.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>

#if defined(__ANDROID__)
    #define VK_USE_PLATFORM_ANDROID_KHR
#elif defined (__APPLE__)
    #define VK_USE_PLATFORM_METAL_EXT
#elif defined(__linux__)
    #define VK_USE_PLATFORM_XCB_KHR
    #define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#define VOLK_IMPLEMENTATION
#include <graphics/vulkan/volk.h>

namespace llri
{
    [[nodiscard]] implementation getImplementation()
    {
        return implementation::Vulkan;
    }
}
