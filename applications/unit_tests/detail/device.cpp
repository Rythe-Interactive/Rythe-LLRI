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

        uint8_t graphicsQueueCount = adapter->queryQueueCount(llri::queue_type::Graphics);
        uint8_t computeQueueCount = adapter->queryQueueCount(llri::queue_type::Compute);
        uint8_t transferQueueCount = adapter->queryQueueCount(llri::queue_type::Transfer);
        
        SUBCASE("Device::getQueue()")
        {
            SUBCASE("[Incorrect usage] Invalid queue_type value")
            {
                CHECK_EQ(device->getQueue(static_cast<llri::queue_type>(UINT_MAX), 0), nullptr);
            }

            SUBCASE("[Incorrect usage] index > number of created queues of this type")
            {
                for (size_t type = 0; type <= static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
                {
                    CHECK_EQ(device->getQueue(static_cast<llri::queue_type>(type), 255), nullptr);
                }
            }

            SUBCASE("[Correct usage] Valid parameters (with queue_descs in mind)")
            {
                llri::Queue* queue = device->getQueue(llri::queue_type::Graphics, 0);
                CHECK_NE(queue, nullptr);
            }
        }

        SUBCASE("Device::createCommandGroup()")
        {
            SUBCASE("[Incorrect usage] cmdGroup == nullptr")
            {
                CHECK_EQ(device->createCommandGroup(llri::queue_type::Graphics, nullptr), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] type is an invalid enum value")
            {
                llri::CommandGroup* cmdGroup;
                CHECK_EQ(device->createCommandGroup(static_cast<llri::queue_type>(UINT_MAX), &cmdGroup), llri::result::ErrorInvalidUsage);
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
                        CHECK_EQ(device->createCommandGroup(static_cast<llri::queue_type>(type), &cmdGroup), llri::result::ErrorInvalidUsage);
                    }
                }
                else
                {
                    const std::string str = std::string("[Correct usage] CommandGroup created with valid parameters for existing ") + llri::to_string(static_cast<llri::queue_type>(type)) + " queue";
                    SUBCASE(str.c_str())
                    {
                        llri::CommandGroup* cmdGroup;
                        CHECK_EQ(device->createCommandGroup(static_cast<llri::queue_type>(type), &cmdGroup), llri::result::Success);
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
                uint8_t count = device->queryQueueCount(static_cast<llri::queue_type>(type));

                if (count > 0)
                {
                    llri::CommandGroup* cmdGroup;
                    REQUIRE_EQ(device->createCommandGroup(static_cast<llri::queue_type>(type), &cmdGroup), llri::result::Success);

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
                CHECK_EQ(device->waitFences(0, &signaledFence, LLRI_TIMEOUT_MAX), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] fences == nullptr")
            {
                CHECK_EQ(device->waitFences(1, nullptr, LLRI_TIMEOUT_MAX), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] a fences[n] == nullptr")
            {
                std::array<llri::Fence*, 2> fences {
                    signaledFence,
                    nullptr
                };
                CHECK_EQ(device->waitFences(fences.size(), fences.data(), LLRI_TIMEOUT_MAX), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] attempting to wait on unsignaled fence(s)")
            {
                // Fence that was never signaled
                llri::Fence* nonSignaledFence;
                REQUIRE_EQ(device->createFence(llri::fence_flag_bits::None, &nonSignaledFence), llri::result::Success);

                CHECK_EQ(device->waitFences(1, &nonSignaledFence, LLRI_TIMEOUT_MAX), llri::result::ErrorNotSignaled);

                device->destroyFence(nonSignaledFence);

                // Fence that was signaled but was waited upon already
                REQUIRE_EQ(device->waitFences(1, &signaledFence, LLRI_TIMEOUT_MAX), llri::result::Success);
                CHECK_EQ(device->waitFences(1, &signaledFence, LLRI_TIMEOUT_MAX), llri::result::ErrorNotSignaled);
            }

            SUBCASE("[Correct usage] valid")
            {
                CHECK_EQ(device->waitFences(1, &signaledFence, LLRI_TIMEOUT_MAX), llri::result::Success);
            }

            device->destroyFence(signaledFence);
        }

        SUBCASE("Device::createSemaphore()")
        {
            SUBCASE("[Incorrect usage] semaphore == nullptr")
            {
                CHECK_EQ(device->createSemaphore(nullptr), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Correct usage] valid parameters")
            {
                llri::Semaphore* semaphore;
                auto r = device->createSemaphore(&semaphore);
                CHECK_UNARY(r == llri::result::Success || r == llri::result::ErrorOutOfDeviceMemory || r == llri::result::ErrorOutOfHostMemory);

                device->destroySemaphore(semaphore);
            }
        }

        SUBCASE("Device::destroySemaphore")
        {
            // nullptr is allowed
            CHECK_NOTHROW(device->destroySemaphore(nullptr));

            // valid pointer is allowed
            llri::Semaphore* semaphore;
            auto r = device->createSemaphore(&semaphore);
            REQUIRE_UNARY(r == llri::result::Success || r == llri::result::ErrorOutOfDeviceMemory || r == llri::result::ErrorOutOfHostMemory);
            if (r == llri::result::Success)
                CHECK_NOTHROW(device->destroySemaphore(semaphore));
        }

        instance->destroyDevice(device);
        llri::destroyInstance(instance);
    }
}
