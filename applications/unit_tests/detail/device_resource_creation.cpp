/**
 * @file device_resource_creation.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <doctest/doctest.h>
#include <helpers.hpp>
#include <future>

TEST_CASE("Device::createResource()")
{
    auto* instance = detail::defaultInstance();
    
    detail::iterateAdapters(instance, [instance](llri::Adapter* adapter) {
        auto* device = detail::defaultDevice(instance, adapter);
        
        llri::resource_desc desc {};

        for (uint8_t node = 0; node < adapter->queryNodeCount(); node++)
        {
            desc.createNodeMask = 1u << node;

            for (uint8_t mask = 0; mask < static_cast<uint8_t>(1 << adapter->queryNodeCount()); mask++)
            {
                desc.visibleNodeMask = mask;

                for (uint8_t type = 0; type <= static_cast<uint8_t>(llri::resource_type::MaxEnum); type++)
                {
                    desc.type = static_cast<llri::resource_type>(type);

                    // we don't test all combinations here because it increases the number of iterations too significantly
                    std::array<llri::resource_usage_flags, 5> flags = {
                        llri::resource_usage_flag_bits::TransferSrc | llri::resource_usage_flag_bits::TransferDst,
                        llri::resource_usage_flag_bits::Sampled, llri::resource_usage_flag_bits::ShaderWrite,
                        llri::resource_usage_flag_bits::ColorAttachment,
                        llri::resource_usage_flag_bits::DepthStencilAttachment | llri::resource_usage_flag_bits::DenyShaderResource
                    };

                    for (auto usageFlag : flags)
                    {
                        desc.usage = usageFlag;

                        for (uint8_t memType = 0; memType <= static_cast<uint8_t>(llri::memory_type::MaxEnum); memType++)
                        {
                            desc.memoryType = static_cast<llri::memory_type>(memType);

                            for (uint8_t resourceState = 0; resourceState <= static_cast<uint8_t>(llri::resource_state::MaxEnum); resourceState++)
                            {
                                desc.initialState = static_cast<llri::resource_state>(resourceState);

                                const std::unordered_set<uint32_t> possibleSizeValues = { 0, 1, std::numeric_limits<uint32_t>::max() };
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
                                        CHECK_UNARY(result == llri::result::Success || result == llri::result::ErrorInvalidUsage || result == llri::result::ErrorOutOfDeviceMemory || result == llri::result::ErrorInvalidNodeMask);
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

                                                std::array<llri::sample_count, 3> sampleCounts {
                                                    llri::sample_count::Count1, llri::sample_count::Count8, llri::sample_count::Count32
                                                };

                                                for (auto sample : sampleCounts)
                                                {
                                                    desc.sampleCount = sample;

                                                    // taking a couple of variations to cover the most relevant cases
                                                    std::array<llri::format, 17> formats {
                                                        llri::format::R8UNorm, llri::format::RG8Norm, llri::format::RGBA8UInt, llri::format::RGBA8sRGB, llri::format::BGRA8UNorm,
                                                        llri::format::RGB10A2UNorm,
                                                        llri::format::R16Int, llri::format::RG16UNorm, llri::format::RGBA16UNorm,
                                                        llri::format::R32UInt, llri::format::RG32UInt, llri::format::RGB32UInt, llri::format::RGBA32UInt,
                                                        llri::format::D16UNorm, llri::format::D24UNormS8UInt, llri::format::D32Float, llri::format::D32FloatS8X24UInt
                                                    };

                                                    for (auto f : formats)
                                                    {
                                                        desc.textureFormat = f;

                                                        llri::Resource* resource = nullptr;
                                                        llri::result result = device->createResource(desc, &resource);
                                                        auto str = to_string(result);
                                                        INFO("result = ", to_string(result).c_str());
                                                        CHECK_UNARY(result == llri::result::Success || result == llri::result::ErrorInvalidUsage || result == llri::result::ErrorOutOfDeviceMemory || result == llri::result::ErrorInvalidNodeMask);
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

        instance->destroyDevice(device);
    });
    
    llri::destroyInstance(instance);
}
