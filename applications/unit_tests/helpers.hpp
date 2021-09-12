/**
 * @file helpers.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */
#pragma once

#include <doctest/doctest.h>
#include <llri/llri.hpp>

namespace helpers
{
    inline llri::Instance* defaultInstance()
    {
        llri::Instance* instance;
        llri::instance_extension ext{ llri::instance_extension_type::DriverValidation, llri::driver_validation_ext { true } };
        const llri::instance_desc desc{ 1, &ext, ""};
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        return instance;
    }

    inline llri::Adapter* selectAdapter(llri::Instance* instance)
    {
        std::vector<llri::Adapter*> adapters;
        REQUIRE_EQ(instance->enumerateAdapters(&adapters), llri::result::Success);
        return adapters[0];
    }

    inline llri::Device* defaultDevice(llri::Instance* instance, llri::Adapter* adapter)
    {
        llri::Device* device = nullptr;

        uint8_t graphicsQueueCount, computeQueueCount, transferQueueCount;
        REQUIRE_EQ(adapter->queryQueueCount(llri::queue_type::Graphics, &graphicsQueueCount), llri::result::Success);
        REQUIRE_EQ(adapter->queryQueueCount(llri::queue_type::Compute, &computeQueueCount), llri::result::Success);
        REQUIRE_EQ(adapter->queryQueueCount(llri::queue_type::Transfer, &transferQueueCount), llri::result::Success);

        std::vector<llri::queue_desc> queues;

        if (graphicsQueueCount > 0)
            queues.push_back(llri::queue_desc{ llri::queue_type::Graphics, llri::queue_priority::Normal });
        if (computeQueueCount > 0)
            queues.push_back(llri::queue_desc{ llri::queue_type::Compute, llri::queue_priority::Normal });
        if (transferQueueCount > 0)
            queues.push_back(llri::queue_desc{ llri::queue_type::Transfer, llri::queue_priority::Normal });

        const llri::device_desc ddesc{ adapter, llri::adapter_features{}, 0, nullptr, static_cast<uint32_t>(queues.size()), queues.data() };
        REQUIRE_EQ(instance->createDevice(ddesc, &device), llri::result::Success);
        return device;
    }

    inline llri::queue_type availableQueueType(llri::Adapter* adapter)
    {
        for (uint8_t type = 0; type <= static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
        {
            uint8_t count = 0;
            adapter->queryQueueCount(static_cast<llri::queue_type>(type), &count);

            if (count > 0)
                return static_cast<llri::queue_type>(type);
        }

        FAIL("No available queue for this adapter");
        return llri::queue_type::MaxEnum;
    }

    inline llri::CommandGroup* defaultCommandGroup(llri::Device* device, llri::queue_type type, uint8_t count)
    {
        llri::CommandGroup* cmdGroup;
        const llri::command_group_desc desc { type, count };
        REQUIRE_EQ(device->createCommandGroup(desc, &cmdGroup), llri::result::Success);
        return cmdGroup;
    }

    inline llri::CommandList* defaultCommandList(llri::CommandGroup* group, uint32_t nodeMask, llri::command_list_usage usage)
    {
        llri::command_list_alloc_desc desc{};
        desc.nodeMask = nodeMask;
        desc.usage = usage;

        llri::CommandList* cmd;
        REQUIRE_EQ(group->allocate(desc, &cmd), llri::result::Success);
        return cmd;
    }
}
