/**
 * @file testsystem.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

/**
 * Sandbox is a testing area for LLRI development.
 * The code written in sandbox should be up to spec but may not contain the best practices or cleanest examples.
 *
 * See the samples for recommended usage and more detailed comments.
 */

#include "testsystem.hpp"

using namespace legion;

#define THROW_IF_FAILED(operation) { \
    auto r = operation; \
    if (r != llri::result::Success) \
    { \
        log::error("LLRI Operation {} returned: {}", #operation, llri::to_string(r)); \
        throw std::runtime_error("LLRI Operation failed"); \
    } \
} \

void callback(llri::message_severity severity, llri::message_source source, const char* message, void* userData)
{
    log::severity sev = log::severity_info;
    switch (severity)
    {
        case llri::message_severity::Verbose:
            sev = log::severity_trace;
            break;
        case llri::message_severity::Info:
            //Even though this semantically maps to info, we'd recommend running this on the trace severity to avoid the excessive info logs that some APIs output
            sev = log::severity_trace;
            break;
        case llri::message_severity::Warning:
            sev = log::severity_warn;
            break;
        case llri::message_severity::Error:
            sev = log::severity_error;
            break;
        case llri::message_severity::Corruption:
            sev = log::severity_error;
    }

    println(sev, "LLRI [{}]: {}", to_string(source), message);
}

void TestSystem::setup()
{
    filter(log::severity_debug);

    log::info("LLRI linked Implementation: {}", llri::to_string(llri::queryImplementation()));

    llri::setMessageCallback(&callback);

    createInstance();
    selectAdapter();
    createDevice();
    createCommandLists();
    createSynchronization();
    createResources();
}

void TestSystem::update(time::span deltaTime)
{
    (void)deltaTime;

    // wait for our frame to be ready
    m_device->waitFence(m_fence, LLRI_TIMEOUT_MAX);

    // record command list
    THROW_IF_FAILED(m_commandGroup->reset());
    const llri::command_list_begin_desc beginDesc {};
    THROW_IF_FAILED(m_commandList->record(beginDesc, [](llri::CommandList* cmd)
    {
    }, m_commandList));

    // submit
    const llri::submit_desc submitDesc { 0, 1, &m_commandList, 0, nullptr, 0, nullptr, m_fence };
    THROW_IF_FAILED(m_graphicsQueue->submit(submitDesc));
}

TestSystem::~TestSystem()
{
    m_device->destroyResource(m_buffer);
    m_device->destroyResource(m_texture);

    m_device->destroySemaphore(m_semaphore);
    m_device->destroyFence(m_fence);
    m_device->destroyCommandGroup(m_commandGroup);

    m_instance->destroyDevice(m_device);
    destroyInstance(m_instance);
}

void TestSystem::createInstance()
{
    //Select Instance Extensions
    std::vector<llri::instance_extension> instanceExtensions;
    if (queryInstanceExtensionSupport(llri::instance_extension_type::DriverValidation))
        instanceExtensions.emplace_back(llri::instance_extension_type::DriverValidation, llri::driver_validation_ext { true });
    if (queryInstanceExtensionSupport(llri::instance_extension_type::GPUValidation))
        instanceExtensions.emplace_back(llri::instance_extension_type::GPUValidation, llri::gpu_validation_ext { true });

    const llri::instance_desc instanceDesc{ (uint32_t)instanceExtensions.size(), instanceExtensions.data(), "sandbox" };

    //Create instance
    THROW_IF_FAILED(llri::createInstance(instanceDesc, &m_instance));
}

void TestSystem::selectAdapter()
{
    //Iterate over adapters
    std::vector<llri::Adapter*> adapters;
    THROW_IF_FAILED(m_instance->enumerateAdapters(&adapters));
    assert(!adapters.empty());

    std::map<int, llri::Adapter*> sortedAdapters;
    for (llri::Adapter* adapter : adapters)
    {
        //Log adapter info
        llri::adapter_info info;
        THROW_IF_FAILED(adapter->queryInfo(&info));

        log::info("Found adapter {}", info.adapterName);
        log::info("\tVendor ID: {}", info.vendorId);
        log::info("\tAdapter ID: {}", info.adapterId);
        log::info("\tAdapter Type: {}", to_string(info.adapterType));

        uint8_t nodeCount = adapter->queryNodeCount();
        log::info("\tAdapter Nodes: {}", nodeCount);

        uint8_t maxGraphicsQueueCount, maxComputeQueueCount, maxTransferQueueCount;
        THROW_IF_FAILED(adapter->queryQueueCount(llri::queue_type::Graphics, &maxGraphicsQueueCount));
        THROW_IF_FAILED(adapter->queryQueueCount(llri::queue_type::Compute, &maxComputeQueueCount));
        THROW_IF_FAILED(adapter->queryQueueCount(llri::queue_type::Transfer, &maxTransferQueueCount));

        log::info("\t Max number of queues: ");
        log::info("\t\t Graphics: {}", maxGraphicsQueueCount);
        log::info("\t\t Compute: {}", maxComputeQueueCount);
        log::info("\t\t Transfer: {}", maxTransferQueueCount);

        uint32_t score = 0;

        //Discrete adapters tend to be more powerful and have more resources so we can decide to pick them
        if (info.adapterType == llri::adapter_type::Discrete)
            score += 1000;

        sortedAdapters[score] = adapter;
    }

    m_adapter = sortedAdapters.begin()->second;
}

void TestSystem::createDevice()
{
    llri::adapter_features selectedFeatures {};

    std::vector<llri::adapter_extension> adapterExtensions;

    std::array<llri::queue_desc, 1> adapterQueues {
        llri::queue_desc { llri::queue_type::Graphics, llri::queue_priority::High } //We can give one or more queues a higher priority
    };

    //Create device
    const llri::device_desc deviceDesc{
        m_adapter, selectedFeatures,
        (uint32_t)adapterExtensions.size(), adapterExtensions.data(),
        (uint32_t)adapterQueues.size(), adapterQueues.data()
    };

    THROW_IF_FAILED(m_instance->createDevice(deviceDesc, &m_device));

    THROW_IF_FAILED(m_device->queryQueue(llri::queue_type::Graphics, 0, &m_graphicsQueue));
}

void TestSystem::createCommandLists()
{
    const llri::command_group_desc groupDesc { llri::queue_type::Graphics, 1 };
    THROW_IF_FAILED(m_device->createCommandGroup(groupDesc, &m_commandGroup));

    const llri::command_list_alloc_desc listDesc { 0, llri::command_list_usage::Direct };
    THROW_IF_FAILED(m_commandGroup->allocate(listDesc, &m_commandList));
}

void TestSystem::createSynchronization()
{
    THROW_IF_FAILED(m_device->createFence(llri::fence_flag_bits::Signaled, &m_fence));
    THROW_IF_FAILED(m_device->createSemaphore(&m_semaphore));
}

void TestSystem::createResources()
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
    textureDesc.format = llri::format::RGBA8sRGB;
    textureDesc.tiling = llri::tiling::Optimal;

    THROW_IF_FAILED(m_device->createResource(textureDesc, &m_texture));
}
