/**
 * @file surface_properties.hpp
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
#elif defined(__linux__)
    #define GLFW_EXPOSE_NATIVE_X11
    #include <X11/Xlib.h>
    #include <X11/Xlib-xcb.h>
    #undef None
    #undef Success
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

inline void impl_testSurfaceCapabilities(llri::Adapter* adapter, llri::SurfaceEXT* surface)
{
    llri::surface_capabilities_ext capabilities;
    CHECK_EQ(adapter->querySurfaceCapabilitiesEXT(surface, &capabilities), llri::result::Success);
    
    CHECK_UNARY(capabilities.minTextureCount > 0);
    CHECK_UNARY(capabilities.maxTextureCount > 0);
    CHECK_UNARY(capabilities.maxTextureCount >= capabilities.minTextureCount);
    
    CHECK_UNARY(capabilities.minExtent.width > 0);
    CHECK_UNARY(capabilities.minExtent.height > 0);
    CHECK_UNARY(capabilities.maxExtent.width > 0);
    CHECK_UNARY(capabilities.maxExtent.height > 0);
    CHECK_UNARY(capabilities.maxExtent.width >= capabilities.minExtent.width);
    CHECK_UNARY(capabilities.maxExtent.height >= capabilities.minExtent.height);
    
    CHECK_UNARY(capabilities.formats.size() > 0);
    for (auto f : capabilities.formats)
        CHECK_UNARY(f <= llri::format::MaxEnum);
    
    CHECK_UNARY(capabilities.presentModes.size() > 0);
    for (auto p : capabilities.presentModes)
        CHECK_UNARY(p <= llri::present_mode_ext::MaxEnum);
    
    CHECK_UNARY(capabilities.usageBits != llri::resource_usage_flag_bits::None);
    CHECK_UNARY(capabilities.usageBits <= llri::resource_usage_flag_bits::All);
}

void testSurfaceCapabilitiesWin32(){
#ifdef _WIN32
    llri::Instance* instance = helpers::createInstanceWithExtension(llri::instance_extension::SurfaceWin32);
    if (!instance)
        return;
    llri::Adapter* adapter = helpers::selectAdapter(instance);
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(960, 540, "unit_tests", nullptr, nullptr);
    if (!window)
        return;
    
    // create surface
    llri::surface_win32_desc_ext surfaceDesc{};
    surfaceDesc.hinstance = GetModuleHandle(NULL);
    surfaceDesc.hwnd = glfwGetWin32Window(window);
    
    llri::SurfaceEXT* surface;
    REQUIRE_EQ(instance->createSurfaceEXT(surfaceDesc, &surface), llri::result::Success);
    
    // do tests
    impl_testSurfaceCapabilities(adapter, surface);
    
    instance->destroySurfaceEXT(surface);
    llri::destroyInstance(instance);
    glfwDestroyWindow(window);
#endif
}

void testSurfaceCapabilitiesCocoa()
{
#ifdef __APPLE__
    llri::Instance* instance = helpers::createInstanceWithExtension(llri::instance_extension::SurfaceCocoa);
    if (!instance)
        return;
    llri::Adapter* adapter = helpers::selectAdapter(instance);
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(960, 540, "unit_tests", nullptr, nullptr);
    if (!window)
        return;
    
    // create surface
    llri::surface_cocoa_desc_ext surfaceDesc{};
    surfaceDesc.nsWindow = glfwGetCocoaWindow(window);
    
    llri::SurfaceEXT* surface;
    REQUIRE_EQ(instance->createSurfaceEXT(surfaceDesc, &surface), llri::result::Success);
    
    // do tests
    impl_testSurfaceCapabilities(adapter, surface);
    
    instance->destroySurfaceEXT(surface);
    llri::destroyInstance(instance);
    glfwDestroyWindow(window);
#endif
}

void testSurfaceCapabilitiesXlib()
{
#ifdef __linux__
    llri::Instance* instance = helpers::createInstanceWithExtension(llri::instance_extension::SurfaceXlib);
    if (!instance)
        return;
    llri::Adapter* adapter = helpers::selectAdapter(instance);
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_X11_XCB_VULKAN_SURFACE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(960, 540, "unit_tests", nullptr, nullptr);
    if (!window)
        return;
    
    // create surface
    llri::surface_xlib_desc_ext surfaceDesc{};
    surfaceDesc.display = glfwGetX11Display();
    surfaceDesc.window = glfwGetX11Window(window);
    
    llri::SurfaceEXT* surface;
    REQUIRE_EQ(instance->createSurfaceEXT(surfaceDesc, &surface), llri::result::Success);
    
    // do tests
    impl_testSurfaceCapabilities(adapter, surface);
    
    instance->destroySurfaceEXT(surface);
    llri::destroyInstance(instance);
    glfwDestroyWindow(window);
#endif
}

void testSurfaceCapabilitiesXcb()
{
#ifdef __linux__
    llri::Instance* instance = helpers::createInstanceWithExtension(llri::instance_extension::SurfaceXcb);
    if (!instance)
        return;
    llri::Adapter* adapter = helpers::selectAdapter(instance);
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_X11_XCB_VULKAN_SURFACE, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(960, 540, "unit_tests", nullptr, nullptr);
    if (!window)
        return;
    
    // create surface
    llri::surface_xcb_desc_ext surfaceDesc{};
    surfaceDesc.connection = XGetXCBConnection(glfwGetX11Display());
    surfaceDesc.window = glfwGetX11Window(window);
    
    llri::SurfaceEXT* surface;
    REQUIRE_EQ(instance->createSurfaceEXT(surfaceDesc, &surface), llri::result::Success);
    
    // do tests
    impl_testSurfaceCapabilities(adapter, surface);
    
    instance->destroySurfaceEXT(surface);
    llri::destroyInstance(instance);
    glfwDestroyWindow(window);
#endif
}

inline void testSurfaceCapabilities()
{
    if (!glfwInit())
        return;
    
    SUBCASE("Simple parameter validation")
    {
        llri::Instance* instance = helpers::defaultInstance();
        llri::Adapter* adapter = helpers::selectAdapter(instance);
                
        CHECK_EQ(adapter->querySurfaceCapabilitiesEXT(nullptr, nullptr), llri::result::ErrorExtensionNotEnabled);

        llri::destroyInstance(instance);
        
#if defined(_WIN32)
        instance = helpers::createInstanceWithExtension(llri::instance_extension::SurfaceWin32);
#elif defined(__APPLE__)
        instance = helpers::createInstanceWithExtension(llri::instance_extension::SurfaceCocoa);
#elif defined(__linux__)
        instance = helpers::createInstanceWithExtension(llri::instance_extension::SurfaceXlib);
#endif
        
        adapter = helpers::selectAdapter(instance);
        
        CHECK_EQ(adapter->querySurfaceCapabilitiesEXT(nullptr, nullptr), llri::result::ErrorInvalidUsage);
        
        llri::surface_capabilities_ext capabilities;
        CHECK_EQ(adapter->querySurfaceCapabilitiesEXT(nullptr, &capabilities), llri::result::ErrorInvalidUsage);
        
        llri::destroyInstance(instance);
    }
    
    SUBCASE("Return values")
    {
        testSurfaceCapabilitiesWin32();
        testSurfaceCapabilitiesCocoa();
        testSurfaceCapabilitiesXlib();
        testSurfaceCapabilitiesXcb();
    }
}
