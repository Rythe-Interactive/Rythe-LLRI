/**
 * @file llri.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <helpers.hpp>
#include <doctest/doctest.h>

struct queue_wrapper
{
    llri::queue_type type;
    uint8_t index;
    llri::Queue* queue;
};

TEST_CASE("Queue")
{
    auto* instance = helpers::defaultInstance();
    auto* adapter = helpers::selectAdapter(instance);
    auto* device = helpers::defaultDevice(instance, adapter);

    // gather all possible queues
    std::vector<queue_wrapper> queues;
    for (uint8_t type = 0; type < static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
    {
        for (size_t i = 0; i < device->queryQueueCount(static_cast<llri::queue_type>(type)); i++)
        {
            llri::Queue* queue;
            REQUIRE_EQ(device->queryQueue(static_cast<llri::queue_type>(type), i, &queue), llri::result::Success);

            queues.push_back({ static_cast<llri::queue_type>(type), static_cast<uint8_t>(i), queue });
        }
    }

    for (size_t node = 0; node < adapter->queryNodeCount(); node++)
    {
        uint32_t nodeMask = 1 << node;
        for (auto& wrapper : queues)
        {
            const std::string str = llri::to_string(wrapper.type) + " Queue " + std::to_string(wrapper.index);

            auto* queue = wrapper.queue;

            auto* group = helpers::defaultCommandGroup(device, helpers::availableQueueType(adapter), 10);
            auto* recordedCmdList = helpers::defaultCommandList(group, nodeMask, llri::command_list_usage::Direct);

            SUBCASE(str.c_str())
            {
                SUBCASE("Queue::submit()")
                {

                }
            }

            device->destroyCommandGroup(group);
        }
    }

    instance->destroyDevice(device);
    llri::destroyInstance(instance);
}
