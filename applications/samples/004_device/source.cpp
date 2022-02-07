/**
 * @file source.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <iostream>

// See 001_validation.
void callback(llri::message_severity severity, llri::message_source source, const char* message, [[maybe_unused]] void* userData)
{
    if (severity <= llri::message_severity::Info)
        return;

    std::cout << "LLRI " << to_string(source) << " " << to_string(severity) << ": " << message << "\n";
}

llri::Instance* createInstance();
llri::Adapter* selectAdapter(llri::Instance* instance);

int main()
{
    llri::setMessageCallback(&callback);

    auto* instance = createInstance();

    // After one or more adapters is selected, a Device may be created.
    // Devices are interfaces for Adapters, and allow you to create resources for the Adapter such as Textures, Buffers, Command Lists, etc.
    auto* adapter = selectAdapter(instance);

    // Describe a set of enabled features.
    // A set of supported features may be queried through Adapter::queryFeatures(), however it is not recommended to pass that exact structure as the enabledFeatures structure.
    // enabling features may come at an additional performance cost as drivers may lose their ability to enable optimizations.
    // so it is recommended to only enable the features that you need/want for your use case.
    llri::adapter_features enabledFeatures {};

    // A device **must** have at least one queue added to its queue desc.
    // No queue of any type is guaranteed to be supported, use Adapter::queryQueueCount() to figure out how many queues are available of a certain type.
    std::array<llri::queue_desc, 1> queues {
        llri::queue_desc { llri::queue_type::Graphics, llri::queue_priority::Normal } // Graphics queues aren't always guaranteed to be available, but in selectAdapter() this sample skips adapters that don't support at least one graphics queue. You may choose for yourself what queues your application will require and select an adapter based on that.
    };

    // Gather all the information from above.
    llri::device_desc desc {
        adapter,
        enabledFeatures,
        0, nullptr, // Similar to Instance extensions, this may be a size and array.
        static_cast<uint32_t>(queues.size()), queues.data()
    };

    // Finally, create the device through Instance::createDevice().
    llri::Device* device;
    llri::result r = instance->createDevice(desc, &device);
    if (r != llri::result::Success)
        return -1;

    std::cout << "Successfully created Device\n";

    // Make sure to clean up created resources.
    instance->destroyDevice(device);
    llri::destroyInstance(instance);
    return 0;
}

// See 000_hello_llri.
llri::Instance* createInstance()
{
    const llri::instance_desc instanceDesc = { 0, nullptr, "device" };

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

    return (*sortedAdapters.begin()).second;
}
