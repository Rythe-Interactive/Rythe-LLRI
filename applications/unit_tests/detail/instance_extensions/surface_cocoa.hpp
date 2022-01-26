/**
 * @file surface_cocoa.hpp
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

inline void testInstanceSurfaceCocoa()
{
    llri::surface_cocoa_desc_ext empty {};
    
    SUBCASE("Extension not enabled")
    {
        llri::instance_desc desc {};
        llri::Instance* instance;
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        
        llri::SurfaceEXT* surface;
        CHECK_EQ(instance->createSurfaceEXT(empty, &surface), llri::result::ErrorExtensionNotEnabled);
        
        llri::destroyInstance(instance);
    }
    
    if (llri::queryInstanceExtensionSupport(llri::instance_extension::SurfaceCocoa))
    {
        glfwInit();
        // disable the default OpenGL context that GLFW creates
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(960, 540, "sandbox", nullptr, nullptr);
        
        llri::instance_extension ext = llri::instance_extension::SurfaceCocoa;
        
        llri::instance_desc desc {};
        desc.numExtensions = 1;
        desc.extensions = &ext;
        
        llri::Instance* instance;
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        
        // surface cant be nullptr
        CHECK_EQ(instance->createSurfaceEXT(empty, nullptr), llri::result::ErrorInvalidUsage);
        
        // nsWindow can't be nullptr
        llri::SurfaceEXT* surface;
        CHECK_EQ(instance->createSurfaceEXT(empty, &surface), llri::result::ErrorInvalidUsage);
        
#ifdef __APPLE__
        // valid
        llri::surface_cocoa_desc_ext sd {};
        sd.nsWindow = glfwGetCocoaWindow(window);
        
        CHECK_EQ(instance->createSurfaceEXT(sd, &surface), llri::result::Success);
        
        instance->destroySurfaceEXT(surface);
#endif
        
        llri::destroyInstance(instance);
        
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}
