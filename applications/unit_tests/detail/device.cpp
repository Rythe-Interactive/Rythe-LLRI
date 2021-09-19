/**
 * @file device.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>
#include <helpers.hpp>
#include <array>

TEST_CASE("Device")
{
    SUBCASE("Functions")
    {
        auto* instance = helpers::defaultInstance();
        auto* adapter = helpers::selectAdapter(instance);
        auto* device = helpers::defaultDevice(instance, adapter);

        uint8_t graphicsQueueCount, computeQueueCount, transferQueueCount;
        REQUIRE_EQ(adapter->queryQueueCount(llri::queue_type::Graphics, &graphicsQueueCount), llri::result::Success);
        REQUIRE_EQ(adapter->queryQueueCount(llri::queue_type::Compute, &computeQueueCount), llri::result::Success);
        REQUIRE_EQ(adapter->queryQueueCount(llri::queue_type::Transfer, &transferQueueCount), llri::result::Success);

        SUBCASE("Device::queryQueue()")
        {
            SUBCASE("[Incorrect usage] Invalid queue_type value")
            {
                llri::Queue* queue;
                CHECK_EQ(device->queryQueue((llri::queue_type)UINT_MAX, 0, &queue), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] index > number of created queues of this type")
            {
                for (size_t type = 0; type <= (uint8_t)llri::queue_type::MaxEnum; type++)
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

            for (size_t type = 0; type <= static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
            {
                uint8_t count = device->queryQueueCount(static_cast<llri::queue_type>(type));

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

            for (size_t type = 0; type <= static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
            {
                uint8_t count = device->queryQueueCount((llri::queue_type)type);

                if (count > 0)
                {
                    llri::CommandGroup* cmdGroup;
                    llri::command_group_desc desc { (llri::queue_type)type, 1 };
                    REQUIRE_EQ(device->createCommandGroup(desc, &cmdGroup), llri::result::Success);

                    CHECK_NOTHROW(device->destroyCommandGroup(cmdGroup));
                }
            }
        }

        SUBCASE("Device::createFence()")
        {
            SUBCASE("[Incorrect usage] Invalid fence flags")
            {
                llri::Fence* fence = nullptr;
                CHECK_EQ(device->createFence(static_cast<llri::fence_flag_bits>(-1), &fence), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] fence == nullptr")
            {
                CHECK_EQ(device->createFence(llri::fence_flag_bits::None, nullptr), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Correct usage] valid parameters")
            {
                std::vector<llri::fence_flags> supportedFlags = {
                    llri::fence_flag_bits::None,
                    llri::fence_flag_bits::Signaled
                };

                for (auto f : supportedFlags)
                {
                    llri::Fence* fence = nullptr;
                    auto r = device->createFence(f, &fence);
                    CHECK_UNARY(r == llri::result::Success || r == llri::result::ErrorOutOfDeviceMemory || r == llri::result::ErrorOutOfHostMemory);

                    if (r == llri::result::Success)
                        device->destroyFence(fence);
                }
            }
        }

        SUBCASE("Device::destroyFence()")
        {
            // nullptr is allowed
            CHECK_NOTHROW(device->destroyFence(nullptr));

            // valid pointer is allowed
            llri::Fence* fence;
            auto r = device->createFence(llri::fence_flag_bits::None, &fence);
            REQUIRE_UNARY(r == llri::result::Success || r == llri::result::ErrorOutOfDeviceMemory || r == llri::result::ErrorOutOfHostMemory);
            if (r == llri::result::Success)
                CHECK_NOTHROW(device->destroyFence(fence));
        }

        SUBCASE("Device::waitFences()")
        {
            llri::Fence* signaledFence;
            REQUIRE_EQ(device->createFence(llri::fence_flag_bits::Signaled, &signaledFence), llri::result::Success);

            SUBCASE("[Incorrect usage] numFences == 0")
            {
                CHECK_EQ(device->waitFences(0, &signaledFence, LLRI_TIMEOUT_INFINITE), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] fences == nullptr")
            {
                CHECK_EQ(device->waitFences(1, nullptr, LLRI_TIMEOUT_INFINITE), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] a fences[n] == nullptr")
            {
                std::array<llri::Fence*, 2> fences {
                    signaledFence,
                    nullptr
                };
                CHECK_EQ(device->waitFences(fences.size(), fences.data(), LLRI_TIMEOUT_INFINITE), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] attempting to wait on unsignaled fence(s)")
            {
                // Fence that was never signaled
                llri::Fence* nonSignaledFence;
                REQUIRE_EQ(device->createFence(llri::fence_flag_bits::None, &nonSignaledFence), llri::result::Success);

                CHECK_EQ(device->waitFences(1, &nonSignaledFence, LLRI_TIMEOUT_INFINITE), llri::result::ErrorNotSignaled);

                device->destroyFence(nonSignaledFence);

                // Fence that was signaled but was waited upon already
                REQUIRE_EQ(device->waitFences(1, &signaledFence, LLRI_TIMEOUT_INFINITE), llri::result::Success);
                CHECK_EQ(device->waitFences(1, &signaledFence, LLRI_TIMEOUT_INFINITE), llri::result::ErrorNotSignaled);
            }

            SUBCASE("[Correct usage] valid")
            {
                CHECK_EQ(device->waitFences(1, &signaledFence, LLRI_TIMEOUT_INFINITE), llri::result::Success);
            }

            device->destroyFence(signaledFence);
        }

        instance->destroyDevice(device);
        llri::destroyInstance(instance);
    }
}
