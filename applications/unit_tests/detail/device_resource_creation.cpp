/**
 * @file device_resource_creation.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>
#include <helpers.hpp>
#include <array>

TEST_CASE("Device::createResource()")
{
    auto* instance = helpers::defaultInstance();
    auto* adapter = helpers::selectAdapter(instance);
    auto* device = helpers::defaultDevice(instance, adapter);

    llri::Resource* failurePlaceholder;
    llri::resource_desc desc;

    SUBCASE("[Incorrect usage] resource == nullptr")
    {
        CHECK_EQ(device->createResource({}, nullptr), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Incorrect usage] desc.createNodeMask has multiple bits set")
    {
        desc.createNodeMask = 1 | 2;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidNodeMask);
    }

    SUBCASE("[Incorrect usage] desc.createNodeMask is not represented by a valid node")
    {
        desc.createNodeMask = UINT_MAX;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidNodeMask);
    }

    SUBCASE("[Incorrect ussage] desc.visibleNodeMask doesn't have at least the same bit set as desc.createNodeMask")
    {
        desc.createNodeMask = 1;
        desc.visibleNodeMask = 2;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidNodeMask);
    }

    SUBCASE("[Incorrect usage] desc.visibleNodeMask has bits set that aren't represented by a valid node")
    {
        desc.createNodeMask = 1;
        desc.visibleNodeMask = UINT_MAX;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidNodeMask);
    }

    for (uint8_t i = 0; i < adapter->queryNodeCount(); i++)
    {
        uint32_t nodeMask = 1 << i;

        const std::string str = "Device node " + std::to_string(nodeMask) + " at index " + std::to_string(i);
        SUBCASE(str.c_str())
        {
            desc.createNodeMask = nodeMask;
            desc.visibleNodeMask = nodeMask;

            SUBCASE("resource_type::Buffer")
            {
                desc.type = llri::resource_type::Buffer;
                CHECK(true);
            }

            SUBCASE("resource_type::Texture1D")
            {
                desc.type = llri::resource_type::Texture1D;
                CHECK(true);
            }

            SUBCASE("resource_type::Texture2D")
            {
                desc.type = llri::resource_type::Texture2D;
                CHECK(true);
            }

            SUBCASE("resource_type::Texture3D")
            {
                desc.memoryType = llri::resource_type::Texture3D;
                CHECK(true);
            }
        }
    }

    instance->destroyDevice(device);
    llri::destroyInstance(instance);
}
