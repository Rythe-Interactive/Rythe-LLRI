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

    llri::resource_desc desc {};

    for (uint8_t node = 0; node < adapter->queryNodeCount(); node++)
    {
        desc.createNodeMask = 1u << node;

        for (uint8_t mask = 0; mask < (1 << adapter->queryNodeCount()); mask++)
        {
            desc.visibleNodeMask = mask;

            for (uint8_t type = 0; type <= static_cast<uint8_t>(llri::resource_type::MaxEnum); type++)
            {
                desc.type = static_cast<llri::resource_type>(type);

                for (uint16_t usage = 0; usage < 128; usage++)
                {
                    desc.usage = usage;

                    for (uint8_t memType = 0; memType <= static_cast<uint8_t>(llri::resource_memory_type::MaxEnum); memType++)
                    {
                        desc.memoryType = static_cast<llri::resource_memory_type>(memType);

                        for (uint8_t resourceState = 0; resourceState <= static_cast<uint8_t>(llri::resource_state::MaxEnum); resourceState++)
                        {
                            desc.initialState = static_cast<llri::resource_state>(resourceState);

                            // note: 0, 1, 1024, and UINT_MAX are arbitrary test values, 2048 is the default max depthOrArrayLayers value and 16384 is the default max width/height value.
                            const std::set<uint32_t> possibleSizeValues = { 0, 1, 1024, 2048, 16384, UINT_MAX };
                            for (auto width : possibleSizeValues)
                            {
                                desc.width = width;

                                if (desc.type == llri::resource_type::Buffer)
                                {
                                    desc.height = 1;
                                    desc.depthOrArrayLayers = 1;

                                    llri::Resource* resource = nullptr;
                                    llri::result result = device->createResource(desc, &resource);
                                    auto str = to_string(result);
                                    INFO("result = ", to_string(result).c_str());
                                    CHECK_UNARY(result == llri::result::Success || result == llri::result::ErrorInvalidUsage || result == llri::result::ErrorOutOfDeviceMemory);
                                    device->destroyResource(resource);

                                    continue;
                                }

                                for (auto height : possibleSizeValues)
                                {
                                    desc.height = height;

                                    for (auto depth : possibleSizeValues)
                                    {
                                        desc.depthOrArrayLayers = static_cast<uint16_t>(depth);

                                        for (auto mip : possibleSizeValues)
                                        {
                                            desc.mipLevels = static_cast<uint16_t>(mip);

                                            for (uint32_t sample = 1; sample <= static_cast<uint32_t>(llri::texture_sample_count::MaxEnum); sample = sample << 1)
                                            {
                                                desc.sampleCount = static_cast<llri::texture_sample_count>(sample);

                                                for (uint32_t format = 0; format <= static_cast<uint32_t>(llri::texture_format::MaxEnum); format++)
                                                {
                                                    desc.format = static_cast<llri::texture_format>(format);

                                                    for (uint8_t tiling = 0; tiling <= static_cast<uint8_t>(llri::texture_tiling::MaxEnum); tiling++)
                                                    {
                                                        desc.tiling = static_cast<llri::texture_tiling>(tiling);

                                                        llri::Resource* resource = nullptr;
                                                        llri::result result = device->createResource(desc, &resource);
                                                        auto str = to_string(result);
                                                        INFO("result = ", to_string(result).c_str());
                                                        CHECK_UNARY(result == llri::result::Success || result == llri::result::ErrorInvalidUsage || result == llri::result::ErrorOutOfDeviceMemory);
                                                        device->destroyResource(resource);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    instance->destroyDevice(device);
    llri::destroyInstance(instance);
}
