/**
 * @file instance.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
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
            llri::instance_extension extension{ llri::instance_extension_type::DriverValidation, llri::driver_validation_ext { false } };
            desc.numExtensions = 1;
            desc.extensions = &extension;

            auto result = llri::createInstance(desc, &instance);
            CHECK_UNARY(result == llri::result::Success || result == llri::result::ErrorExtensionNotSupported);
        }
        
        SUBCASE("[Incorrect usage] invalid extension type")
        {
            llri::instance_extension extension{ (llri::instance_extension_type)UINT_MAX, llri::driver_validation_ext { false } };

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
            //Any of the following is a valid result value for enumerateAdapters
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

            //Adapter count should stay consistent
            CHECK_EQ(count, adapters.size());
        }

        llri::destroyInstance(instance);
    }

    TEST_CASE("Instance::createDevice()")
    {
        llri::Instance* instance = helpers::defaultInstance();

        std::vector<llri::Adapter*> adapters;
        REQUIRE_EQ(instance->enumerateAdapters(&adapters), llri::result::Success);

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

        for (auto* adapter : adapters)
        {
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
                //Reserved for future use (no current extensions supported to test this)
            }

            SUBCASE("[Incorrect usage] invalid extension type")
            {
                //Check reserved for future use. Since there are no adapter extensions, createDevice is currently not expected to iterate over extensions and check for their support, so this remains commented out for now.
                
                //llri::adapter_extension extension{};
                //extension.type = static_cast<llri::adapter_extension_type>(UINT_MAX);

                //ddesc.numExtensions = 1;
                //ddesc.extensions = &extension;
                //CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorExtensionNotSupported);
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
                llri::queue_desc queueDesc { (llri::queue_type)UINT_MAX, llri::queue_priority::Normal };
                ddesc.numQueues = 1;
                ddesc.queues = &queueDesc;
                CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] invalid queue_priority")
            {
                llri::queue_desc queueDesc { llri::queue_type::Graphics, (llri::queue_priority)UINT_MAX };
                ddesc.numQueues = 1;
                ddesc.queues = &queueDesc;
                CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] more queues of a type than supported")
            {
                for (size_t type = 0; type < static_cast<uint8_t>(llri::queue_type::MaxEnum); type++)
                {
                    //Get max number of queues
                    uint8_t count;
                    REQUIRE_EQ(adapter->queryQueueCount((llri::queue_type)type, &count), llri::result::Success);

                    //Create more queues than supported
                    std::vector<llri::queue_desc> queues(count + 1, llri::queue_desc{ static_cast<llri::queue_type>(type), llri::queue_priority::Normal });
                    ddesc.numQueues = (uint32_t)queues.size();
                    ddesc.queues = queues.data();

                    //Should be invalid
                    CHECK_EQ(instance->createDevice(ddesc, &device), llri::result::ErrorInvalidUsage);
                }
            }

            SUBCASE("[Correct usage] maximum number of queues")
            {
                std::vector<llri::queue_desc> queues;

                std::unordered_map<llri::queue_type, uint8_t> maxQueueCounts{
                    { llri::queue_type::Graphics, 0 },
                    { llri::queue_type::Compute, 0 },
                    { llri::queue_type::Transfer, 0 }
                };
                adapter->queryQueueCount(llri::queue_type::Graphics, &maxQueueCounts[llri::queue_type::Graphics]);
                adapter->queryQueueCount(llri::queue_type::Compute, &maxQueueCounts[llri::queue_type::Compute]);
                adapter->queryQueueCount(llri::queue_type::Transfer, &maxQueueCounts[llri::queue_type::Transfer]);

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
        }

        llri::destroyInstance(instance);
    }

    TEST_CASE("Instance::destroyDevice()")
    {
        llri::Instance* instance = helpers::defaultInstance();

        std::vector<llri::Adapter*> adapters;
        REQUIRE_EQ(instance->enumerateAdapters(&adapters), llri::result::Success);

        for (auto* adapter : adapters)
        {
            SUBCASE("[Correct usage] device != nullptr")
            {
                llri::Device* device = nullptr;
                llri::queue_desc queue { llri::queue_type::Graphics, llri::queue_priority::Normal }; //at least one graphics queue is practically always available
                llri::device_desc ddesc{ adapter, llri::adapter_features{}, 0, nullptr, 1, &queue};

                REQUIRE_EQ(instance->createDevice(ddesc, &device), llri::result::Success);
                CHECK_NOTHROW(instance->destroyDevice(device));
            }

            SUBCASE("[Correct usage] device == nullptr")
            {
                CHECK_NOTHROW(instance->destroyDevice(nullptr));
            }
        }
    }
}
