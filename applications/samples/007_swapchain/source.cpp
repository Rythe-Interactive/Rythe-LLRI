/**
 * @file source.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <iostream>
#include <algorithm>

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

// See 001_validation.
void callback(llri::message_severity severity, llri::message_source source, const char* message, [[maybe_unused]] void* userData)
{
    if (severity <= llri::message_severity::Info)
        return;

    std::cout << "LLRI " << to_string(source) << " " << to_string(severity) << ": " << message << "\n";
}

llri::Instance* createInstance();
llri::Adapter* selectAdapter(llri::Instance* instance, llri::SurfaceEXT* surface);
llri::Device* createDevice(llri::Instance* instance, llri::Adapter* adapter);
llri::CommandGroup* createCommandGroup(llri::Device* device);
llri::CommandList* allocateCommandList(llri::CommandGroup* group);
llri::Queue* getQueue(llri::Device* device);

int main()
{
    llri::setMessageCallback(&callback);

    auto* instance = createInstance(); // for this example, note the extra extension added in createInstance()

    // This sample displays how you can use a SurfaceEXT and a SwapchainEXT to render output to a window
    // (in this case GLFW but any other library may be used as long as it can provide native window pointers)

    // simple glfw initialization
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // disable the default OpenGL context that GLFW creates
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // this sample doesn't handle window resizing
    
    bool useXcb = queryInstanceExtensionSupport(llri::instance_extension::SurfaceXcb);
    glfwWindowHint(GLFW_X11_XCB_VULKAN_SURFACE, useXcb); // prefer xcb on linux
    
    GLFWwindow* window = glfwCreateWindow(960, 540, "007_swapchain", nullptr, nullptr);

    // LLRI surfaces are one of the very few features that are platform-dependent
    // surfaces cross the bridge between the platform-agnostic API and the
    // platform-dependent windowing systems.
    // This is usually the ony platform-dependent code that needs to be written for LLRI.
    llri::SurfaceEXT* surface;
    
    // most platform-dependent constructors just pass a native window pointer
    // on to internal LLRI functions.
    // in some cases extra data specific to the platform must be provided.
    
#if defined(_WIN32)
    llri::surface_win32_desc_ext surfaceDesc{};
    surfaceDesc.hinstance = GetModuleHandle(NULL);
    surfaceDesc.hwnd = glfwGetWin32Window(window);

    llri::result result = instance->createSurfaceEXT(surfaceDesc, &surface);
#elif defined(__APPLE__)
    llri::surface_cocoa_desc_ext surfaceDesc{};
    surfaceDesc.nsWindow = glfwGetCocoaWindow(window);

    llri::result result = instance->createSurfaceEXT(surfaceDesc, &surface);
#elif defined(__linux__)
    llri::result result;
    if (useXcb) // prefer xcb on linux when possible
    {
        llri::surface_xcb_desc_ext surfaceDesc{};
        surfaceDesc.connection = XGetXCBConnection(glfwGetX11Display());
        surfaceDesc.window = glfwGetX11Window(window);

        result = instance->createSurfaceEXT(surfaceDesc, &surface);
    }
    else
    {
        llri::surface_xlib_desc_ext surfaceDesc{};
        surfaceDesc.display = glfwGetX11Display();
        surfaceDesc.window = glfwGetX11Window(window);

        result = instance->createSurfaceEXT(surfaceDesc, &surface);
    }
#else
#error platform not yet supported in this sample
#endif

    if (result != llri::result::Success)
        throw std::runtime_error("Failed to create llri::SurfaceEXT");

    auto* adapter = selectAdapter(instance, surface); // note the extra feature checks
    auto* device = createDevice(instance, adapter); // note the extra extension added in device
    auto* group = createCommandGroup(device);
    [[maybe_unused]] auto* list = allocateCommandList(group);
    [[maybe_unused]] auto* queue = getQueue(device);

    // pick valid swapchain settings based on the surface's capabilities
    llri::surface_capabilities_ext surfaceCapabilities;
    result = adapter->querySurfaceCapabilitiesEXT(surface, &surfaceCapabilities);
    if (result != llri::result::Success)
        throw std::runtime_error("Failed to query surface capabilities");

    // sRGB gives better color accuracy so we'll prefer that but if that's not an option we'll simply default to the first surface format
    llri::format selectedSurfaceFormat;
    if (std::find(surfaceCapabilities.textureFormats.begin(), surfaceCapabilities.textureFormats.end(), llri::format::BGRA8sRGB) != surfaceCapabilities.textureFormats.end())
        selectedSurfaceFormat = llri::format::BGRA8sRGB;
    else
        selectedSurfaceFormat = surfaceCapabilities.textureFormats[0];

    // describe how the swapchain should be created -
    // with the surface, and the various properties limited by the surface's queried capabilities.
    llri::swapchain_desc_ext swapchainDesc{};
    swapchainDesc.queue = queue;
    swapchainDesc.surface = surface;
    swapchainDesc.textureFormat = selectedSurfaceFormat;
    swapchainDesc.textureExtent = {
        std::clamp(960u, surfaceCapabilities.minTextureExtent.width, surfaceCapabilities.maxTextureExtent.width),
        std::clamp(540u, surfaceCapabilities.minTextureExtent.height, surfaceCapabilities.maxTextureExtent.height)
    };
    swapchainDesc.textureCount = std::clamp(3u, surfaceCapabilities.minTextureCount, surfaceCapabilities.maxTextureCount);
    swapchainDesc.textureUsage = llri::resource_usage_flag_bits::TransferDst;
    swapchainDesc.presentMode = llri::present_mode_ext::Fifo;

    llri::SwapchainEXT* swapchain = nullptr;
    result = device->createSwapchainEXT(swapchainDesc, &swapchain);
    if (result != llri::result::Success)
        throw std::runtime_error("Failed to create llri::SwapchainEXT");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    // wait till the gpu is done with its submitted operations prior to destroying
    queue->waitIdle();
    
    device->destroySwapchainEXT(swapchain);
    device->destroyCommandGroup(group);
    instance->destroyDevice(device);
    instance->destroySurfaceEXT(surface);
    llri::destroyInstance(instance);
    return 0;
}

// See 000_hello_llri.
llri::Instance* createInstance()
{
    std::vector<llri::instance_extension> extensions;
    if (queryInstanceExtensionSupport(llri::instance_extension::DriverValidation))
        extensions.emplace_back(llri::instance_extension::DriverValidation);
    if (queryInstanceExtensionSupport(llri::instance_extension::GPUValidation))
        extensions.emplace_back(llri::instance_extension::GPUValidation);
    
#if defined(_WIN32)
    if (queryInstanceExtensionSupport(llri::instance_extension::SurfaceWin32) == false)
        throw std::runtime_error("Win32 Surface support is required for this sample");
    
    extensions.push_back(llri::instance_extension::SurfaceWin32);
#elif defined(__APPLE__)
    if (queryInstanceExtensionSupport(llri::instance_extension::SurfaceCocoa) == false)
        throw std::runtime_error("Cocoa Surface support is required for this sample");
    
    extensions.push_back(llri::instance_extension::SurfaceCocoa);
#elif defined(__linux__)
    // prefer xcb
    if (queryInstanceExtensionSupport(llri::instance_extension::SurfaceXcb))
    {
        extensions.push_back(llri::instance_extension::SurfaceXcb);
        printf("using Xcb instead of Xlib\n");
    }
    else if (queryInstanceExtensionSupport(llri::instance_extension::SurfaceXlib))
        extensions.push_back(llri::instance_extension::SurfaceXlib);
    else
        throw std::runtime_error("Xlib or Xcb Surface support is required for this sample");
#else
#error platform not yet supported in this sample
#endif

    const llri::instance_desc instanceDesc = { static_cast<uint32_t>(extensions.size()), extensions.data(), "007_swapchain" };

    llri::Instance* instance;
    const llri::result r = llri::createInstance(instanceDesc, &instance);
    if (r != llri::result::Success)
        throw std::runtime_error("Failed to create LLRI instance");

    return instance;
}

// See 003_adapter_selection.
llri::Adapter* selectAdapter(llri::Instance* instance, llri::SurfaceEXT* surface)
{
    std::vector<llri::Adapter*> adapters;
    llri::result r = instance->enumerateAdapters(&adapters);
    if (r != llri::result::Success)
        throw std::runtime_error("Failed to enumerate adapters");

    std::unordered_map<int, llri::Adapter*> sortedAdapters;
    for (auto* adapter : adapters)
    {
        // rendering to a surface requires the swapchain adapter extension.
        if (adapter->queryExtensionSupport(llri::adapter_extension::Swapchain) == false)
            continue;

        // the adapter needs to be able to present to the created surface
        bool canPresent;
        r = adapter->querySurfacePresentSupportEXT(surface, llri::queue_type::Graphics, &canPresent);
        if (r != llri::result::Success)
            continue;
        
        if (canPresent == false)
            continue;

        llri::adapter_info info = adapter->queryInfo();
        
        uint8_t graphicsQueueCount = adapter->queryQueueCount(llri::queue_type::Graphics);
        // Skip this Adapter if it has no graphics queue available.
        if (graphicsQueueCount == 0)
            continue;

        int score = 0;

        // Discrete adapters tend to be more performant so we'll rate them much higher.
        if (info.adapterType == llri::adapter_type::Discrete)
            score += 1000;

        sortedAdapters.emplace(score, adapter);
    }

    if (sortedAdapters.empty())
        throw std::runtime_error("Failed to find a suitable adapter");

    return (*sortedAdapters.begin()).second;
}

// See 004_device
llri::Device* createDevice(llri::Instance* instance, llri::Adapter* adapter)
{
    llri::adapter_features enabledFeatures{};

    std::array<llri::queue_desc, 1> queues{
        // This sample requires/picks an Adapter with a Graphics queue, but you may choose
        // to use different queues in your use case.
        llri::queue_desc { llri::queue_type::Graphics, llri::queue_priority::Normal }
    };

    std::array<llri::adapter_extension, 1> extensions {
        llri::adapter_extension::Swapchain
    };

    llri::device_desc desc{
        adapter,
        enabledFeatures,
        static_cast<uint32_t>(extensions.size()), extensions.data(),
        static_cast<uint32_t>(queues.size()), queues.data()
    };

    llri::Device* device;
    llri::result r = instance->createDevice(desc, &device);
    if (r != llri::result::Success)
        throw std::runtime_error("Failed to create llri::Device");

    return device;
}

// see 005_commands
llri::CommandGroup* createCommandGroup(llri::Device* device)
{
    llri::CommandGroup* group;
    if (device->createCommandGroup(llri::queue_type::Graphics, &group) != llri::result::Success)
        throw std::runtime_error("Failed to create llri::CommandGroup");

    return group;
}

// see 005_commands
llri::CommandList* allocateCommandList(llri::CommandGroup* group)
{
    llri::command_list_alloc_desc alloc{ };
    alloc.nodeMask = 0;
    alloc.usage = llri::command_list_usage::Direct;

    llri::CommandList* list;
    if (group->allocate(alloc, &list) != llri::result::Success)
        throw std::runtime_error("Failed to allocate llri::CommandList");

    return list;
}

// see 006_queue_submit
llri::Queue* getQueue(llri::Device* device)
{
    llri::Queue* output = device->getQueue(llri::queue_type::Graphics, 0);
    return output;
}
