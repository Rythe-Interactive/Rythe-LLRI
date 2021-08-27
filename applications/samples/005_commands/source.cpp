/**
 * @file source.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <iostream>
#include <array>

// See 001_validation.
void callback(llri::validation_callback_severity severity, llri::validation_callback_source source, const char* message, void* userData)
{
    if (severity <= llri::validation_callback_severity::Info)
        return;

    std::cout << "LLRI " << to_string(source) << " " << to_string(severity) << ": " << message << "\n";
}

llri::Instance* createInstance();
llri::Adapter* selectAdapter(llri::Instance* instance);
llri::Device* createDevice(llri::Instance* instance, llri::Adapter* adapter);

int main()
{
    auto* instance = createInstance();
    auto* adapter = selectAdapter(instance);
    auto* device = createDevice(instance, adapter);

    // This sample expands upon 004_device by showing how the newly created device can be used to create and record CommandLists.

    // CommandLists can only be allocated through a CommandGroup.
    // CommandGroups are responsible for managing the device memory allocated for each CommandList.
    llri::command_group_desc groupDesc{};
    groupDesc.count = 1; // CommandGroups must know pre-emptively how many CommandLists they can allocate to reserve the appropriate device memory.
    groupDesc.type = llri::queue_type::Graphics; // There **must** be an Adapter queue of this type available for this to be valid.

    llri::CommandGroup* group;
    if (device->createCommandGroup(groupDesc, &group) != llri::result::Success)
        throw;

    // When CommandLists are allocated through their CommandGroup, the group maintains full ownership over the CommandList(s).
    llri::command_list_alloc_desc alloc{ };
    alloc.usage = llri::command_list_usage::Direct; // This CommandList will be directly submitted to Queues

    // This CommandList **may** be free'd through CommandGroup::free() but this is
    // not required. Allocated CommandLists will be free'd by CommandGroup upon destruction.
    llri::CommandList* list;
    if (group->allocate(alloc, &list) != llri::result::Success)
        throw;

    // After allocation, CommandLists are in the command_list_state::Empty state, which means that they
    // can be used for recording.
    llri::command_list_begin_desc beginDesc{};
    // CommandList::begin() puts the CommandList into the command_list_state::Recording state.
    // in this state, commands may be recorded into the CommandList.
    if (list->begin(beginDesc) != llri::result::Success)
        throw;

    // Record commands here.

    // Once recording is done, call CommandList::end() to signal that you're done recording.
    // This puts the CommandList in the command_list_state::Ready state, which means that it
    // is ready to be submitted to a queue.
    if (list->end() != llri::result::Success)
        throw;

    // When a CommandList has previously been recorded, it can not be re-recorded until the
    // CommandGroup resets it back to the command_list_state::Empty state.
    // The CommandList can not be in use when this is called.
    if (group->reset() != llri::result::Success)
        throw;

    // Now that the CommandList is reset, it may be used to record again
    // A convenient alternative to begin() and end() is CommandList::record():
    llri::command_list_begin_desc beginDesc2{};
    // record() takes a function and optionally the function's parameters.
    list->record(beginDesc2, [](llri::CommandList* cmd) {
        // Within the function passed, you may record commands.
        // record() simply calls begin(desc), function(args), end().
    }, list);

    // Make sure to clean up created resources.
    device->destroyCommandGroup(group);
    instance->destroyDevice(device);
    llri::destroyInstance(instance);
    return 0;
}

// See 000_hello_llri.
llri::Instance* createInstance()
{
    const llri::instance_desc instanceDesc = {
        0, nullptr, "validation",
        llri::validation_callback_desc {
            &callback,
            nullptr
        }
    };

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

    std::map<int, llri::Adapter*> sortedAdapters;
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
