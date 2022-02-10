/**
 * @file instance.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>
#include <helpers.hpp>

TEST_SUITE("Instance")
{
    TEST_CASE("createInstance()")
    {
        CHECK(llri::createInstance({}, nullptr) == llri::result::ErrorInvalidUsage);

        llri::Instance* instance = nullptr;
        llri::instance_desc desc { 0, nullptr, ""};

        SUBCASE("[Incorrect usage] numExtensions > instance_extensions::MaxEnum")
        {
            desc.numExtensions = std::numeric_limits<uint32_t>::max();
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorExceededLimit);
        }
        
        SUBCASE("[Incorrect usage] numExtensions > 0 && extensions == nullptr")
        {
            desc.numExtensions = 1;
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorInvalidUsage);
        }

        SUBCASE("[Correct usage] numExtensions == 0 && extensions == nullptr")
        {
            desc.numExtensions = 0;
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        }

        SUBCASE("[Correct usage] numExtensions > 0 && extensions != nullptr")
        {
            auto ext = llri::instance_extension::DriverValidation;
            desc.numExtensions = 1;
            desc.extensions = &ext;

            auto result = llri::createInstance(desc, &instance);
            CHECK_UNARY(result == llri::result::Success || result == llri::result::ErrorExtensionNotSupported);
        }
        
        SUBCASE("[Incorrect usage] invalid extension type")
        {
            auto extension = static_cast<llri::instance_extension>(std::numeric_limits<uint8_t>::max());

            desc.numExtensions = 1;
            desc.extensions = &extension;

            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::ErrorExtensionNotSupported);
        }

        INFO("Extension specific tests are done in \"Instance Extensions\"");

        SUBCASE("[Correct usage] applicationName == nullptr")
        {
            desc.applicationName = nullptr;
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        }

        SUBCASE("[Correct usage] applicationName != nullptr")
        {
            desc.applicationName = "Test";
            CHECK_EQ(llri::createInstance(desc, &instance), llri::result::Success);
        }

        llri::destroyInstance(instance);
    }

    TEST_CASE("destroyInstance()")
    {
        SUBCASE("[Correct usage] instance == nullptr")
        {
            CHECK_NOTHROW(llri::destroyInstance(nullptr));
        }

        SUBCASE("[Correct usage] instance != nullptr")
        {
            llri::Instance* instance;
            const llri::instance_desc desc{};
            REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);

            CHECK_NOTHROW(llri::destroyInstance(instance));
        }
    }

    TEST_CASE("Instance::enumerateAdapters")
    {
        llri::Instance* instance = helpers::defaultInstance();

        SUBCASE("[Incorrect usage] adapters == nullptr")
        {
            CHECK_EQ(instance->enumerateAdapters(nullptr), llri::result::ErrorInvalidUsage);
        }

        SUBCASE("[Correct usage] adapters != nullptr")
        {
            std::vector<llri::Adapter*> adapters;
            const llri::result result = instance->enumerateAdapters(&adapters);
            // Any of the following is a valid result value for enumerateAdapters
            CHECK_UNARY(
                result == llri::result::Success ||
                result == llri::result::ErrorOutOfHostMemory ||
                result == llri::result::ErrorOutOfDeviceMemory ||
                result == llri::result::ErrorInitializationFailed
            );
        }

        SUBCASE("[Correct usage] multiple iterations")
        {
            std::vector<llri::Adapter*> adapters;
            llri::result result;

            result = instance->enumerateAdapters(&adapters);
            auto count = adapters.size();
            CHECK_UNARY( 
                result == llri::result::Success ||
                result == llri::result::ErrorOutOfHostMemory ||
                result == llri::result::ErrorOutOfDeviceMemory ||
                result == llri::result::ErrorInitializationFailed
            );

            result = instance->enumerateAdapters(&adapters);
            CHECK_UNARY(
                result == llri::result::Success ||
                result == llri::result::ErrorOutOfHostMemory ||
                result == llri::result::ErrorOutOfDeviceMemory ||
                result == llri::result::ErrorInitializationFailed
            );

            // Adapter count should stay consistent
            CHECK_EQ(count, adapters.size());
        }

        llri::destroyInstance(instance);
    }

    TEST_CASE("Instance::createDevice()")
    {
        llri::Instance* instance = helpers::defaultInstance();

        SUBCASE("[Incorrect usage] device == nullptr")
        {
            llri::device_desc ddesc{};
            CHECK_EQ(instance->createDevice(ddesc, nullptr), llri::result::ErrorInvalidUsage);
        }

        SUBCASE("[Incorrect usage] adapter == nullptr")
        {
            llri::Device* device = nullptr;
            llri::device_desc ddesc{};
            CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
        }
        
        helpers::iterateAdapters(instance, [=](llri::Adapter* adapter) {
            llri::Device* device = nullptr;
            llri::device_desc ddesc{ adapter, llri::adapter_features{}, 0, nullptr, 0, nullptr };

            llri::queue_desc queue { llri::queue_type::Graphics, llri::queue_priority::Normal };

            SUBCASE("[Incorrect usage] numExtensions > 0 && extensions == nullptr")
            {
                ddesc.numExtensions = 1;
                ddesc.extensions = nullptr;

                ddesc.numQueues = 1;
                ddesc.queues = &queue;

                CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Correct usage] numExtensions == 0 && extensions == nullptr")
            {
                ddesc.numExtensions = 0;
                ddesc.extensions = nullptr;

                ddesc.numQueues = 1;
                ddesc.queues = &queue;

                auto r = instance->createDevice(ddesc, &device);
                CHECK_UNARY(r == llri::result::Success || r == llri::result::ErrorDeviceLost);
            }

            SUBCASE("[Correct usage] numExtensions > 0 && extensions != nullptr")
            {
                // Reserved for future use (no current extensions supported to test this)
            }

            SUBCASE("[Incorrect usage] invalid extension type")
            {
                auto extension = static_cast<llri::adapter_extension>(std::numeric_limits<uint8_t>::max());

                ddesc.numExtensions = 1;
                ddesc.extensions = &extension;
                CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorExtensionNotSupported);
            }

            SUBCASE("[Incorrect usage] numQueues == 0")
            {
                CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] extensions == nullptr")
            {
                ddesc.numQueues = 1;
                CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Correct usage] numQueues > 0 && queues != nullptr")
            {
                ddesc.numQueues = 1;
                ddesc.queues = &queue;
                auto r = instance->createDevice(ddesc, &device);
                CHECK_UNARY(r == llri::result::Success || r == llri::result::ErrorDeviceLost);
            }

            SUBCASE("[Correct usage] high priority queue")
            {
                llri::queue_desc queueDesc { llri::queue_type::Graphics, llri::queue_priority::High };

                ddesc.numQueues = 1;
                ddesc.queues = &queueDesc;
                auto r = instance->createDevice(ddesc, &device);
                CHECK_UNARY(r == llri::result::Success || r == llri::result::ErrorDeviceLost);
            }

            SUBCASE("[Incorrect usage] invalid queue_type")
            {
                llri::queue_desc queueDesc { static_cast<llri::queue_type>(std::numeric_limits<uint8_t>::max()), llri::queue_priority::Normal };
                ddesc.numQueues = 1;
                ddesc.queues = &queueDesc;
                CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] invalid queue_priority")
            {
                llri::queue_desc queueDesc { llri::queue_type::Graphics, static_cast<llri::queue_priority>(std::numeric_limits<uint8_t>::max()) };
                ddesc.numQueues = 1;
                ddesc.queues = &queueDesc;
                CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] more queues of a type than supported")
            {
                for (size_t type = 0; type < static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
                {
                    // Get max number of queues
                    uint8_t count = adapter->queryQueueCount(static_cast<llri::queue_type>(type));

                    // Create more queues than supported
                    std::vector<llri::queue_desc> queues(count + 1, llri::queue_desc{ static_cast<llri::queue_type>(type), llri::queue_priority::Normal });
                    ddesc.numQueues = static_cast<uint32_t>(queues.size());
                    ddesc.queues = queues.data();

                    // Should be invalid
                    CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
                }
            }

            SUBCASE("[Correct usage] maximum number of queues")
            {
                std::vector<llri::queue_desc> queues;

                std::unordered_map<llri::queue_type, uint8_t> maxQueueCounts{
                    { llri::queue_type::Graphics, adapter->queryQueueCount(llri::queue_type::Graphics) },
                    { llri::queue_type::Compute, adapter->queryQueueCount(llri::queue_type::Compute) },
                    { llri::queue_type::Transfer, adapter->queryQueueCount(llri::queue_type::Transfer) }
                };
                
                for (uint8_t type = 0; type <= static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
                {
                    for (uint8_t i = 0; i < maxQueueCounts[static_cast<llri::queue_type>(type)]; i++)
                        queues.push_back(llri::queue_desc { static_cast<llri::queue_type>(type), llri::queue_priority::High });
                }

                ddesc.numQueues = static_cast<uint32_t>(queues.size());
                ddesc.queues = queues.data();

                auto r = instance->createDevice(ddesc, &device);
                CHECK_UNARY(r == llri::result::Success || r == llri::result::ErrorDeviceLost);
            }

            INFO("Extension specific tests are done in \"Device Extensions\"");

            instance->destroyDevice(device);
        });

        llri::destroyInstance(instance);
    }

    TEST_CASE("Instance::destroyDevice()")
    {
        llri::Instance* instance = helpers::defaultInstance();
            
        helpers::iterateAdapters(instance, [=](llri::Adapter* adapter) {
            SUBCASE("[Correct usage] device != nullptr")
            {
                llri::Device* device = nullptr;
                llri::queue_desc queue { llri::queue_type::Graphics, llri::queue_priority::Normal }; // at least one graphics queue is practically always available
                llri::device_desc ddesc{ adapter, llri::adapter_features{}, 0, nullptr, 1, &queue};

                REQUIRE_EQ(instance->createDevice(ddesc, &device), llri::result::Success);
                CHECK_NOTHROW(instance->destroyDevice(device));
            }

            SUBCASE("[Correct usage] device == nullptr")
            {
                CHECK_NOTHROW(instance->destroyDevice(nullptr));
            }
        });
    }
}
