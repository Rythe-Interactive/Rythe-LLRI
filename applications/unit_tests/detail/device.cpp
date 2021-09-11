/**
 * @file device.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>
#include <helpers.hpp>

TEST_CASE("Device")
{
    SUBCASE("Functions")
    {
        auto* instance = helpers::defaultInstance();
        auto* adapter = helpers::selectAdapter(instance);
        auto* device = helpers::defaultDevice(instance, adapter);

        uint8_t graphicsQueueCount, computeQueueCount, transferQueueCount;
        adapter->queryQueueCount(llri::queue_type::Graphics, &graphicsQueueCount);
        adapter->queryQueueCount(llri::queue_type::Compute, &computeQueueCount);
        adapter->queryQueueCount(llri::queue_type::Transfer, &transferQueueCount);

        SUBCASE("Device::queryQueue()")
        {
            SUBCASE("[Incorrect usage] Invalid queue_type value")
            {
                llri::Queue* queue;
                CHECK_EQ(device->queryQueue((llri::queue_type)UINT_MAX, 0, &queue), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] index > number of created queues of this type")
            {
                for (uint8_t type = 0; type <= (uint8_t)llri::queue_type::MaxEnum; type++)
                {
                    llri::Queue* queue;
                    CHECK_EQ(device->queryQueue((llri::queue_type)type, 255, &queue), llri::result::ErrorInvalidUsage);
                }
            }

            SUBCASE("[Incorrect usage] queue == nullptr")
            {
                CHECK_EQ(device->queryQueue(llri::queue_type::Graphics, 0, nullptr), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Correct usage] Valid parameters (with queue_descs in mind)")
            {
                llri::Queue* queue;
                CHECK_EQ(device->queryQueue(llri::queue_type::Graphics, 0, &queue), llri::result::Success);
            }
        }

        SUBCASE("Device::createCommandGroup()")
        {
            SUBCASE("[Incorrect usage] cmdGroup == nullptr")
            {
                llri::command_group_desc desc { llri::queue_type::Graphics, 1 };
                CHECK_EQ(device->createCommandGroup(desc, nullptr), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] desc.type is an invalid enum value")
            {
                llri::CommandGroup* cmdGroup;
                llri::command_group_desc desc { (llri::queue_type)UINT_MAX, 1 };
                CHECK_EQ(device->createCommandGroup(desc, &cmdGroup), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] desc.count == 0")
            {
                llri::CommandGroup* cmdGroup;
                llri::command_group_desc desc { llri::queue_type::Graphics, 0 };
                CHECK_EQ(device->createCommandGroup(desc, &cmdGroup), llri::result::ErrorInvalidUsage);
            }

            for (uint8_t type = 0; type <= static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
            {
                uint8_t count;
                adapter->queryQueueCount((llri::queue_type)type, &count);

                if (count == 0)
                {
                    const std::string str = std::string("[Incorrect usage] CommandGroup created for queue_type ") + llri::to_string(static_cast<llri::queue_type>(type)) + " which has no supported queues.";
                    SUBCASE(str.c_str())
                    {
                        llri::CommandGroup* cmdGroup;
                        llri::command_group_desc desc { (llri::queue_type)type, 1 };
                        CHECK_EQ(device->createCommandGroup(desc, &cmdGroup), llri::result::ErrorInvalidUsage);
                    }
                }
                else
                {
                    const std::string str = std::string("[Correct usage] CommandGroup created with valid parameters for existing ") + llri::to_string(static_cast<llri::queue_type>(type)) + " queue";
                    SUBCASE(str.c_str())
                    {
                        llri::CommandGroup* cmdGroup;
                        llri::command_group_desc desc { (llri::queue_type)type, 1 };
                        CHECK_EQ(device->createCommandGroup(desc, &cmdGroup), llri::result::Success);
                        device->destroyCommandGroup(cmdGroup);
                    }
                }
            }
        }

        SUBCASE("Device::destroyCommandGroup()")
        {
            CHECK_NOTHROW(device->destroyCommandGroup(nullptr));

            for (uint8_t type = 0; type <= static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
            {
                uint8_t count;
                REQUIRE_EQ(adapter->queryQueueCount((llri::queue_type)type, &count), llri::result::Success);

                if (count > 0)
                {
                    llri::CommandGroup* cmdGroup;
                    llri::command_group_desc desc { (llri::queue_type)type, 1 };
                    REQUIRE_EQ(device->createCommandGroup(desc, &cmdGroup), llri::result::Success);

                    CHECK_NOTHROW(device->destroyCommandGroup(cmdGroup));
                }
            }
        }

        instance->destroyDevice(device);
        llri::destroyInstance(instance);
    }
}
