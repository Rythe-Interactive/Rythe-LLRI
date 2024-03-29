/**
 * @file surface_xcb.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>
#include <helpers.hpp>

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define VC_EXTRALEAN
    #define NOMINMAX
    #include <Windows.h>
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
    #define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
    #define GLFW_EXPOSE_NATIVE_X11
    #include <X11/Xlib.h>
    #include <X11/Xlib-xcb.h>
    #undef None
    #undef Success
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

inline void testInstanceSurfaceXcb()
{
    llri::surface_xcb_desc_ext empty {};
    
    // check with extension not enabled
    llri::instance_desc desc {};
    llri::Instance* instance;
    REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);
    
    llri::SurfaceEXT* surface;
    CHECK_EQ(instance->createSurfaceEXT(empty, &surface), llri::result::ErrorExtensionNotEnabled);
    
    llri::destroyInstance(instance);

    // check with extension enabled
    if (llri::queryInstanceExtensionSupport(llri::instance_extension::SurfaceXcb))
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_X11_XCB_VULKAN_SURFACE, GLFW_TRUE);
        GLFWwindow* window = glfwCreateWindow(960, 540, "sandbox", nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            return;
        }
        
        llri::instance_extension ext = llri::instance_extension::SurfaceXcb;
        
        desc = {};
        desc.numExtensions = 1;
        desc.extensions = &ext;
        
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        
        // surface cant be nullptr
        CHECK_EQ(instance->createSurfaceEXT(empty, nullptr), llri::result::ErrorInvalidUsage);
        
        // nsWindow can't be nullptr
        CHECK_EQ(instance->createSurfaceEXT(empty, &surface), llri::result::ErrorInvalidUsage);
        
#ifdef __linux__
        // valid
        llri::surface_xcb_desc_ext sd {};
        sd.connection = XGetXCBConnection(glfwGetX11Display());
        sd.window = glfwGetX11Window(window);
        
        CHECK_EQ(instance->createSurfaceEXT(sd, &surface), llri::result::Success);
        
        instance->destroySurfaceEXT(surface);
#endif
        
        llri::destroyInstance(instance);
        
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}
