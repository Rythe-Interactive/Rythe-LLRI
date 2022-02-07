/**
 * @file surface_properties.hpp
 * Copyright (c) 2021 Leon Brands
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

inline void impl_testSurfaceCapabilities(llri::Adapter* adapter, llri::SurfaceEXT* surface)
{
    llri::surface_capabilities_ext capabilities;
    CHECK_EQ(adapter->querySurfaceCapabilitiesEXT(surface, nullptr), llri::result::ErrorInvalidUsage);
    CHECK_EQ(adapter->querySurfaceCapabilitiesEXT(nullptr, &capabilities), llri::result::ErrorInvalidUsage);
    
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

inline void impl_testSurfacePresentSupport(llri::Adapter* adapter, llri::SurfaceEXT* surface)
{
    bool support;
    CHECK_EQ(adapter->querySurfacePresentSupportEXT(nullptr, llri::queue_type::Graphics, &support), llri::result::ErrorInvalidUsage);
    CHECK_EQ(adapter->querySurfacePresentSupportEXT(surface, llri::queue_type::Graphics, nullptr), llri::result::ErrorInvalidUsage);
    CHECK_EQ(adapter->querySurfacePresentSupportEXT(surface, static_cast<llri::queue_type>(std::numeric_limits<uint8_t>::max()), &support), llri::result::ErrorInvalidUsage);
    
    for (uint8_t i = 0; i <= static_cast<uint8_t>(llri::queue_type::MaxEnum); i++)
    {
        CHECK_EQ(adapter->querySurfacePresentSupportEXT(surface, static_cast<llri::queue_type>(i), &support), llri::result::Success);
        CHECK_UNARY(support == 0 || support == 1); // should always be true or false explicitly
    }
}

void testSurfacePropertiesWin32(){
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
    impl_testSurfacePresentSupport(adapter, surface);
    
    instance->destroySurfaceEXT(surface);
    llri::destroyInstance(instance);
    glfwDestroyWindow(window);
#endif
}

void testSurfacePropertiesCocoa()
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
    impl_testSurfacePresentSupport(adapter, surface);
    
    instance->destroySurfaceEXT(surface);
    llri::destroyInstance(instance);
    glfwDestroyWindow(window);
#endif
}

void testSurfacePropertiesXlib()
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
    impl_testSurfacePresentSupport(adapter, surface);
    
    instance->destroySurfaceEXT(surface);
    llri::destroyInstance(instance);
    glfwDestroyWindow(window);
#endif
}

void testSurfacePropertiesXcb()
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
    impl_testSurfacePresentSupport(adapter, surface);
    
    instance->destroySurfaceEXT(surface);
    llri::destroyInstance(instance);
    glfwDestroyWindow(window);
#endif
}

inline void testSurfaceProperties()
{
    if (!glfwInit())
        return;
    
    SUBCASE("extension not enabled")
    {
        llri::Instance* instance = helpers::defaultInstance();
        llri::Adapter* adapter = helpers::selectAdapter(instance);
                
        llri::surface_capabilities_ext capabilities;
        bool support;
        
        CHECK_EQ(adapter->querySurfaceCapabilitiesEXT(nullptr, &capabilities), llri::result::ErrorExtensionNotEnabled);
        CHECK_EQ(adapter->querySurfacePresentSupportEXT(nullptr, llri::queue_type::Graphics, &support), llri::result::ErrorExtensionNotEnabled);
        
        llri::destroyInstance(instance);
    }
    
    SUBCASE("extension enabled")
    {
        testSurfacePropertiesWin32();
        testSurfacePropertiesCocoa();
        testSurfacePropertiesXlib();
        testSurfacePropertiesXcb();
    }
}
