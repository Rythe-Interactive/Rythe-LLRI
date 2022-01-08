/**
 * @file source.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <iostream>

// See 001_validation.
void callback(llri::message_severity severity, llri::message_source source, const char* message, void* userData)
{
    if (severity <= llri::message_severity::Info)
        return;

    std::cout << "LLRI " << to_string(source) << " " << to_string(severity) << ": " << message << "\n";
}

llri::Instance* createInstance();
llri::Adapter* selectAdapter(llri::Instance* instance);
llri::Device* createDevice(llri::Instance* instance, llri::Adapter* adapter);
llri::CommandGroup* createCommandGroup(llri::Device* device);
llri::CommandList* allocateCommandList(llri::CommandGroup* group);

int main()
{
    llri::setMessageCallback(&callback);

    auto* instance = createInstance();
    auto* adapter = selectAdapter(instance);
    auto* device = createDevice(instance, adapter);
    auto* group = createCommandGroup(device);
    auto* list = allocateCommandList(group);

    // This sample expands upon 005_commands by showing how the recorded CommandLists can be submitted to the device's queue.

    // We start out by getting the queue from the previously created device
    llri::Queue* queue = device->getQueue(llri::queue_type::Graphics, 0);

    // Commands must be recorded (in the "Ready" state) before being able to be submitted to a queue.
    const llri::command_list_begin_desc begin{};
    list->record(begin, [](llri::CommandList* cmd) {
        // Record commands
    }, list);

    // After recording, we can submit the CommandList(s) to the queue:
    llri::submit_desc submitDesc{};
    submitDesc.numCommandLists = 1;
    submitDesc.commandLists = &list;
    queue->submit(submitDesc);

    // CommandGroups and their lists can not be reset or destroyed as long as the command list is still in use on the gpu.
    // There are a couple ways of synchronizing events, which will be covered in later samples.
    // This sample uses the much more simple Queue::waitIdle() which blocks the cpu thread until all commands are done.
    queue->waitIdle();

    device->destroyCommandGroup(group);
    instance->destroyDevice(device);
    llri::destroyInstance(instance);
    return 0;
}

// See 000_hello_llri.
llri::Instance* createInstance()
{
    const llri::instance_desc instanceDesc = { 0, nullptr, "queue_submit" };

    llri::Instance* instance;
    const llri::result r = llri::createInstance(instanceDesc, &instance);
    if (r != llri::result::Success)
        return nullptr;

    return instance;
}

// See 003_adapter_selection.
llri::Adapter* selectAdapter(llri::Instance* instance)
{
    std::vector<llri::Adapter*> adapters;
    llri::result r = instance->enumerateAdapters(&adapters);
    if (r != llri::result::Success)
        return nullptr;

    std::unordered_map<int, llri::Adapter*> sortedAdapters;
    for (auto* adapter : adapters)
    {
        llri::adapter_info info;
        r = adapter->queryInfo(&info);
        if (r != llri::result::Success)
            return nullptr;

        uint8_t graphicsQueueCount;
        r = adapter->queryQueueCount(llri::queue_type::Graphics, &graphicsQueueCount);
        if (r != llri::result::Success)
            return nullptr;

        // Skip this Adapter if it has no graphics queue available.
        if (graphicsQueueCount == 0)
            continue;

        int score = 0;

        // Discrete adapters tend to be more performant so we'll rate them much higher.
        if (info.adapterType == llri::adapter_type::Discrete)
            score += 1000;

        sortedAdapters.emplace(score, adapter);
    }

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

    llri::device_desc desc{
        adapter,
        enabledFeatures,
        0, nullptr,
        queues.size(), queues.data()
    };

    llri::Device* device;
    llri::result r = instance->createDevice(desc, &device);
    if (r != llri::result::Success)
        return nullptr;

    return device;
}

// see 005_commands
llri::CommandGroup* createCommandGroup(llri::Device* device)
{
    llri::CommandGroup* group;
    if (device->createCommandGroup(llri::queue_type::Graphics, &group) != llri::result::Success)
        return nullptr;

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
        throw;

    return list;
}
