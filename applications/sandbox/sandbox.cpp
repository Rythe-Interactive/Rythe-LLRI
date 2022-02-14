/**
 * @file sandbox.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <cstdio>
#include <cassert>
#include <llri/llri.hpp>

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

/**
 * Sandbox is a testing area for LLRI development.
 * The code written in sandbox should be up to spec but may not contain the best practices or cleanest examples.
 *
 * See the samples for recommended usage and more detailed comments.
 */

#define THROW_IF_FAILED(operation) { \
    auto r = operation; \
    if (r != llri::result::Success) \
    { \
        printf("LLRI Operation { %s } returned: { %s }", #operation, llri::to_string(r).c_str()); \
        throw std::runtime_error("LLRI Operation failed"); \
    } \
} \

void callback(llri::message_severity severity, llri::message_source source, const char* message, [[maybe_unused]] void* userData)
{
    switch (severity)
    {
        case llri::message_severity::Verbose:
            return;
        case llri::message_severity::Info:
            // Even though this semantically maps to info, we'd recommend running this on the trace severity to avoid the excessive info logs that some APIs output
            return;
        case llri::message_severity::Warning:
            printf("Warning: ");
            break;
        case llri::message_severity::Error:
            printf("Error: ");
            break;
        case llri::message_severity::Corruption:
            printf("Corruption error: ");
    }

    printf("LLRI [%s]: %s\n", to_string(source).c_str(), message);
}

GLFWwindow* m_window = nullptr;
llri::Instance* m_instance = nullptr;
llri::SurfaceEXT* m_surface = nullptr;
llri::SwapchainEXT* m_swapchain = nullptr;
bool m_useXcb = false;

llri::Adapter* m_adapter = nullptr;
llri::Device* m_device = nullptr;

llri::Queue* m_graphicsQueue = nullptr;

llri::CommandGroup* m_commandGroup = nullptr;
llri::CommandList* m_commandList = nullptr;

llri::Fence* m_fence = nullptr;
llri::Semaphore* m_semaphore = nullptr;

llri::Resource* m_buffer = nullptr;
llri::Resource* m_texture = nullptr;

void createInstance();
void createSurface();
void selectAdapter();
void createDevice();
void createCommandLists();
void createSynchronization();
void createResources();
void createSwapchain();

int main()
{
    printf("LLRI linked Implementation: %s\n", llri::to_string(llri::getImplementation()).c_str());

    llri::setMessageCallback(&callback);

    createInstance();
    createSurface();
    selectAdapter();
    createDevice();
    createCommandLists();
    createSynchronization();
    createResources();
    createSwapchain();
    
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        
        // wait for our frame to be ready
        m_device->waitFence(m_fence, LLRI_TIMEOUT_MAX);

        // record command list
        THROW_IF_FAILED(m_commandGroup->reset());
        const llri::command_list_begin_desc beginDesc {};
        THROW_IF_FAILED(m_commandList->record(beginDesc, [](llri::CommandList* cmd)
        {
            cmd->resourceBarrier({
                llri::resource_barrier::read_write(m_buffer),
                llri::resource_barrier::transition(m_texture, llri::resource_state::TransferDst, llri::resource_state::ShaderReadOnly)
            });

            cmd->resourceBarrier(
                llri::resource_barrier::transition(m_texture, llri::resource_state::ShaderReadOnly, llri::resource_state::TransferDst)
            );
        }, m_commandList));

        // submit
        const llri::submit_desc submitDesc { 0, 1, &m_commandList, 0, nullptr, 0, nullptr, m_fence };
        THROW_IF_FAILED(m_graphicsQueue->submit(submitDesc));
    }
    
    m_graphicsQueue->waitIdle();
    
    m_device->destroyResource(m_buffer);
    m_device->destroyResource(m_texture);

    m_device->destroySemaphore(m_semaphore);
    m_device->destroyFence(m_fence);
    m_device->destroyCommandGroup(m_commandGroup);

    m_instance->destroyDevice(m_device);
    
    m_instance->destroySurfaceEXT(m_surface);
    llri::destroyInstance(m_instance);
    
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void createInstance()
{
    // Select Instance Extensions
    std::vector<llri::instance_extension> instanceExtensions;
    if (queryInstanceExtensionSupport(llri::instance_extension::DriverValidation))
        instanceExtensions.emplace_back(llri::instance_extension::DriverValidation);
    if (queryInstanceExtensionSupport(llri::instance_extension::GPUValidation))
        instanceExtensions.emplace_back(llri::instance_extension::GPUValidation);

#if defined(_WIN32)
    if (queryInstanceExtensionSupport(llri::instance_extension::SurfaceWin32) == false)
        throw std::runtime_error("Win32 Surface support is required for this sample");
    
    instanceExtensions.push_back(llri::instance_extension::SurfaceWin32);
#elif defined(__APPLE__)
    if (queryInstanceExtensionSupport(llri::instance_extension::SurfaceCocoa) == false)
        throw std::runtime_error("Cocoa Surface support is required for this sample");
    
    instanceExtensions.push_back(llri::instance_extension::SurfaceCocoa);
#elif defined(__linux__)
    // prefer xcb
    if (queryInstanceExtensionSupport(llri::instance_extension::SurfaceXcb))
    {
        instanceExtensions.push_back(llri::instance_extension::SurfaceXcb);
        m_useXcb = true;
        printf("using Xcb instead of Xlib\n");
    }
    else if (queryInstanceExtensionSupport(llri::instance_extension::SurfaceXlib))
        instanceExtensions.push_back(llri::instance_extension::SurfaceXlib);
    else
        throw std::runtime_error("Xlib or Xcb Surface support is required for this sample");
#else
#error platform not yet supported in this sample
#endif
    
    const llri::instance_desc instanceDesc{ static_cast<uint32_t>(instanceExtensions.size()), instanceExtensions.data(), "sandbox" };

    // Create instance
    THROW_IF_FAILED(llri::createInstance(instanceDesc, &m_instance));
}

void createSurface()
{
    glfwInit();
    // disable the default OpenGL context that GLFW creates
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_X11_XCB_VULKAN_SURFACE, m_useXcb); // enable xcb if we can
    m_window = glfwCreateWindow(960, 540, "sandbox", nullptr, nullptr);

    // LLRI surfaces
#if defined(_WIN32)
    llri::surface_win32_desc_ext surfaceDesc{};
    surfaceDesc.hinstance = GetModuleHandle(NULL);
    surfaceDesc.hwnd = glfwGetWin32Window(m_window);

    THROW_IF_FAILED(m_instance->createSurfaceEXT(surfaceDesc, &m_surface));
#elif defined(__APPLE__)
    llri::surface_cocoa_desc_ext surfaceDesc{};
    surfaceDesc.nsWindow = glfwGetCocoaWindow(m_window);

    THROW_IF_FAILED(m_instance->createSurfaceEXT(surfaceDesc, &m_surface));
#elif defined(__linux__)
    if (m_useXcb)
    {
        llri::surface_xcb_desc_ext surfaceDesc{};
        surfaceDesc.connection = XGetXCBConnection(glfwGetX11Display());
        surfaceDesc.window = glfwGetX11Window(m_window);

        THROW_IF_FAILED(m_instance->createSurfaceEXT(surfaceDesc, &m_surface));
    }
    else
    {
        llri::surface_xlib_desc_ext surfaceDesc{};
        surfaceDesc.display = glfwGetX11Display();
        surfaceDesc.window = glfwGetX11Window(m_window);

        THROW_IF_FAILED(m_instance->createSurfaceEXT(surfaceDesc, &m_surface));
    }
#else
#error platform not yet supported in this sample
#endif
}

void selectAdapter()
{
    // Iterate over adapters
    std::vector<llri::Adapter*> adapters;
    THROW_IF_FAILED(m_instance->enumerateAdapters(&adapters));
    assert(!adapters.empty());

    std::unordered_map<int, llri::Adapter*> sortedAdapters;
    for (llri::Adapter* adapter : adapters)
    {
        // Log adapter info
        llri::adapter_info info = adapter->queryInfo();

        printf("Found adapter %s\n", info.adapterName.c_str());
        printf("\tVendor ID: %u\n", info.vendorId);
        printf("\tAdapter ID: %u\n", info.adapterId);
        printf("\tAdapter Type: %s\n", to_string(info.adapterType).c_str());

        uint8_t nodeCount = adapter->queryNodeCount();
        printf("\tAdapter Nodes: %u\n", nodeCount);

        uint8_t maxGraphicsQueueCount = adapter->queryQueueCount(llri::queue_type::Graphics);
        uint8_t maxComputeQueueCount = adapter->queryQueueCount(llri::queue_type::Compute);
        uint8_t maxTransferQueueCount = adapter->queryQueueCount(llri::queue_type::Transfer);

        printf("\tMax number of queues: \n");
        printf("\t\tGraphics: %u\n", maxGraphicsQueueCount);
        printf("\t\tCompute: %u\n", maxComputeQueueCount);
        printf("\t\tTransfer: %u\n", maxTransferQueueCount);
        
        // rendering to a surface requires the swapchain adapter extension.
        if (adapter->queryExtensionSupport(llri::adapter_extension::Swapchain) == false)
            continue;
        
        // Require support for using/presenting with Graphics to our surface
        bool support;
        THROW_IF_FAILED(adapter->querySurfacePresentSupportEXT(m_surface, llri::queue_type::Graphics, &support));
        printf("\tSurface graphics present support: %i\n", support);
        if (!support)
            continue;
        
        THROW_IF_FAILED(adapter->querySurfacePresentSupportEXT(m_surface, llri::queue_type::Compute, &support));
        printf("\tSurface compute present support: %i\n", support);

        uint32_t score = 0;

        // Discrete adapters tend to be more powerful and have more resources so we can decide to pick them
        if (info.adapterType == llri::adapter_type::Discrete)
            score += 1000;
        
        sortedAdapters[score] = adapter;
    }

    m_adapter = sortedAdapters.begin()->second;
}

void createDevice()
{
    llri::adapter_features selectedFeatures {};

    std::vector<llri::adapter_extension> adapterExtensions {
        llri::adapter_extension::Swapchain
    };

    std::array<llri::queue_desc, 1> adapterQueues {
        llri::queue_desc { llri::queue_type::Graphics, llri::queue_priority::High } // We can give one or more queues a higher priority
    };

    // Create device
    const llri::device_desc deviceDesc{
        m_adapter, selectedFeatures,
        static_cast<uint32_t>(adapterExtensions.size()), adapterExtensions.data(),
        static_cast<uint32_t>(adapterQueues.size()), adapterQueues.data()
    };

    THROW_IF_FAILED(m_instance->createDevice(deviceDesc, &m_device));

    m_graphicsQueue = m_device->getQueue(llri::queue_type::Graphics, 0);
}

void createCommandLists()
{
    THROW_IF_FAILED(m_device->createCommandGroup(llri::queue_type::Graphics, &m_commandGroup));

    const llri::command_list_alloc_desc listDesc { 0, llri::command_list_usage::Direct };
    THROW_IF_FAILED(m_commandGroup->allocate(listDesc, &m_commandList));
}

void createSynchronization()
{
    THROW_IF_FAILED(m_device->createFence(llri::fence_flag_bits::Signaled, &m_fence));
    THROW_IF_FAILED(m_device->createSemaphore(&m_semaphore));
}

void createResources()
{
    llri::resource_desc bufferDesc = llri::resource_desc::buffer(llri::resource_usage_flag_bits::ShaderWrite, llri::memory_type::Local, llri::resource_state::ShaderReadWrite, 64);

    THROW_IF_FAILED(m_device->createResource(bufferDesc, &m_buffer));

    llri::resource_desc textureDesc;
    textureDesc.createNodeMask = 0;
    textureDesc.visibleNodeMask = 0;
    textureDesc.type = llri::resource_type::Texture2D;
    textureDesc.usage = llri::resource_usage_flag_bits::TransferDst | llri::resource_usage_flag_bits::Sampled;
    textureDesc.memoryType = llri::memory_type::Local;
    textureDesc.initialState = llri::resource_state::TransferDst;
    textureDesc.width = 1028;
    textureDesc.height = 1028;
    textureDesc.depthOrArrayLayers = 1;
    textureDesc.mipLevels = 1;
    textureDesc.sampleCount = llri::sample_count::Count1;
    textureDesc.textureFormat = llri::format::RGBA8sRGB;

    THROW_IF_FAILED(m_device->createResource(textureDesc, &m_texture));
}

void createSwapchain()
{
    llri::surface_capabilities_ext capabilities;
    THROW_IF_FAILED(m_adapter->querySurfaceCapabilitiesEXT(m_surface, &capabilities));
    
    printf("Surface capabilities:\n");
    printf("\tMin texture count: %u\n", capabilities.minTextureCount);
    printf("\tMax texture count: %u\n", capabilities.maxTextureCount);
    printf("\tMin extent: %s\n", llri::to_string(capabilities.minTextureExtent).c_str());
    printf("\tMax extent: %s\n", llri::to_string(capabilities.maxTextureExtent).c_str());
    
    printf("\tSupported formats: ");
    for(auto f : capabilities.textureFormats)
        printf("%s, ", llri::to_string(f).c_str());
    printf("\n");
    
    printf("\tSupported present modes: ");
    for (auto p : capabilities.presentModes)
        printf("%s, ", llri::to_string(p).c_str());
    printf("\n");
    
    printf("\tSupported usage bits: %s\n", llri::to_string(capabilities.textureUsage).c_str());
    
    // sRGB gives better color accuracy so we'll prefer that but if that's not an option we'll simply default to the first surface format
    llri::format selectedSurfaceFormat;
    if (std::find(capabilities.textureFormats.begin(), capabilities.textureFormats.end(), llri::format::BGRA8sRGB) != capabilities.textureFormats.end())
        selectedSurfaceFormat = llri::format::BGRA8sRGB;
    else
        selectedSurfaceFormat = capabilities.textureFormats[0];

    // describe how the swapchain should be created -
    // with the surface, and the various properties limited by the surface's queried capabilities.
    llri::swapchain_desc_ext swapchainDesc{};
    swapchainDesc.surface = m_surface;
    swapchainDesc.textureFormat = selectedSurfaceFormat;
    swapchainDesc.textureExtent = {
        std::clamp(960u, capabilities.minTextureExtent.width, capabilities.maxTextureExtent.width),
        std::clamp(540u, capabilities.minTextureExtent.height, capabilities.maxTextureExtent.height)
    };
    swapchainDesc.textureCount = std::clamp(3u, capabilities.minTextureCount, capabilities.maxTextureCount);
    swapchainDesc.textureUsage = llri::resource_usage_flag_bits::TransferDst;
    swapchainDesc.presentMode = llri::present_mode_ext::Fifo;

    THROW_IF_FAILED(m_device->createSwapchainEXT(swapchainDesc, &m_swapchain));
}
