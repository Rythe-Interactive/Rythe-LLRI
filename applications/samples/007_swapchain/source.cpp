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
#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
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

    glfwInit();
    // disable the default OpenGL context that GLFW creates
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // this sample doesn't handle window resizing
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(960, 540, "007_swapchain", nullptr, nullptr);

    // LLRI surfaces
#if defined(_WIN32)
    llri::surface_win32_desc_ext surfaceDesc{};
    surfaceDesc.hinstance = GetModuleHandle(NULL);
    surfaceDesc.hwnd = glfwGetWin32Window(window);
#elif defined(__APPLE__)
    llri::surface_cocoa_desc_ext surfaceDesc{};
    surfaceDesc.nsWindow = glfwGetCocoaWindow(window);
#else
#error platform not yet supported in this sample
#endif

    llri::SurfaceEXT* surface = nullptr;
    auto result = instance->createSurfaceEXT(surfaceDesc, &surface);
    if (result != llri::result::Success)
        throw std::runtime_error("Failed to create llri::SurfaceEXT");

    auto* adapter = selectAdapter(instance, surface);
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
    if (std::find(surfaceCapabilities.formats.begin(), surfaceCapabilities.formats.end(), llri::format::BGRA8sRGB) != surfaceCapabilities.formats.end())
        selectedSurfaceFormat = llri::format::BGRA8sRGB;
    else
        selectedSurfaceFormat = surfaceCapabilities.formats[0];

    // GPU operations need to wait before swapchain textures are ready
    // this can be synchronized with a fence or semaphore, but semaphores are preferred since the operations tend to be
    // gpu-based (e.g. command list submission).
    llri::Semaphore* semaphore;
    if (device->createSemaphore(&semaphore) != llri::result::Success)
        throw std::runtime_error("Failed to create semaphore");

    // describe how the swapchain should be created -
    // with the surface, and the various properties limited by the surface's queried properties.
    llri::swapchain_desc_ext swapchainDesc{};
    swapchainDesc.surface = surface;
    swapchainDesc.format = selectedSurfaceFormat;
    swapchainDesc.extent = {
        std::clamp(960u, surfaceCapabilities.minExtent.width, surfaceCapabilities.maxExtent.width),
        std::clamp(540u, surfaceCapabilities.minExtent.height, surfaceCapabilities.maxExtent.height)
    };
    swapchainDesc.textureCount = std::clamp(3u, surfaceCapabilities.minTextureCount, surfaceCapabilities.maxTextureCount);
    swapchainDesc.usage = llri::resource_usage_flag_bits::TransferDst; // transferDst is required for clear operations
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

    device->destroySemaphore(semaphore);
    
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
#if defined(_WIN32)
    if (queryInstanceExtensionSupport(llri::instance_extension::SurfaceWin32) == false)
        throw std::runtime_error("Win32 Surface support is required for this sample");
    
    std::array<llri::instance_extension, 2> extensions {
        llri::instance_extension::SurfaceWin32,
        llri::instance_extension::DriverValidation,
    };
#elif defined(__APPLE__)
    if (queryInstanceExtensionSupport(llri::instance_extension::SurfaceCocoa) == false)
        throw std::runtime_error("Win32 Surface support is required for this sample");
    
    std::array<llri::instance_extension, 2> extensions {
        llri::instance_extension::SurfaceCocoa,
        llri::instance_extension::DriverValidation,
    };
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
        extensions.size(), extensions.data(),
        queues.size(), queues.data()
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
