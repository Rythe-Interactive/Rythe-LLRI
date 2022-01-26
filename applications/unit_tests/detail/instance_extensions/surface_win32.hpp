/**
 * @file surface_win32.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>
#include <helpers.hpp>

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

inline void testInstanceSurfaceWin32()
{
    llri::surface_win32_desc_ext empty {};
    
    // check with extension not enabled
    llri::instance_desc desc {};
    llri::Instance* instance;
    REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);
    
    llri::SurfaceEXT* surface;
    CHECK_EQ(instance->createSurfaceEXT(empty, &surface), llri::result::ErrorExtensionNotEnabled);
    
    llri::destroyInstance(instance);
    
    // check with extension enabled
    if (llri::queryInstanceExtensionSupport(llri::instance_extension::SurfaceWin32))
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(960, 540, "sandbox", nullptr, nullptr);
        
        llri::instance_extension ext = llri::instance_extension::SurfaceWin32;
        
        llri::instance_desc desc {};
        desc.numExtensions = 1;
        desc.extensions = &ext;
        
        llri::Instance* instance;
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        
        // surface cant be nullptr
        CHECK_EQ(instance->createSurfaceEXT(empty, nullptr), llri::result::ErrorInvalidUsage);
        
        llri::SurfaceEXT* surface;
        llri::surface_win32_desc_ext sd {};
        
        // hwnd can't be nullptr
        CHECK_EQ(instance->createSurfaceEXT(sd, &surface), llri::result::ErrorInvalidUsage);
        
#ifdef _WIN32
        // hinstance cant be nullptr
        sd.hwnd = glfwGetWin32Window(window);
        CHECK_EQ(instance->createSurfaceEXT(sd, &surface), llri::result::ErrorInvalidUsage);

        sd.hinstance = GetModuleHandle(NULL);
        CHECK_EQ(instance->createSurfaceEXT(sd, &surface), llri::result::Success);
        
        instance->destroySurfaceEXT(surface);
#endif
        
        llri::destroyInstance(instance);
        
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}
