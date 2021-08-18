/**
 * @file device.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>

TEST_CASE("Device")
{
    SUBCASE("Functions")
    {
        llri::Instance* instance;
        const llri::instance_desc desc{};
        REQUIRE_EQ(llri::createInstance(desc, &instance), llri::result::Success);

        std::vector<llri::Adapter*> adapters;
        REQUIRE_EQ(instance->enumerateAdapters(&adapters), llri::result::Success);

        llri::Device* device = nullptr;
        llri::queue_desc queueDesc { llri::queue_type::Graphics, llri::queue_priority::Normal }; //at least one graphics queue is practically always available
        llri::device_desc ddesc{ adapters[0], llri::adapter_features{}, 0, nullptr, 1, &queueDesc};
        REQUIRE_EQ(instance->createDevice(ddesc, &device), llri::result::Success);

        SUBCASE("Device::queryQueue()")
        {
            SUBCASE("[Incorrect usage] Invalid queue_type value")
            {
                llri::Queue* queue;
                CHECK_EQ(device->queryQueue((llri::queue_type)UINT_MAX, 0, &queue), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Incorrect usage] index > number of created queues of this type")
            {
                for (uint8_t type = 0; type < (uint8_t)llri::queue_type::MaxEnum; type++)
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
    }
}
