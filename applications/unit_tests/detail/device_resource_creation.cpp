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
    desc.type = llri::resource_type::Texture2D;
    desc.createNodeMask = 0;
    desc.visibleNodeMask = 0;
    desc.usage = {};
    desc.memoryType = llri::resource_memory_type::Local;
    desc.initialState = llri::resource_state::General;
    desc.width = 1028;
    desc.height = 1;
    desc.depthOrArrayLayers = 1;
    desc.mipLevels = 1;
    desc.sampleCount = llri::texture_sample_count::Count1;
    desc.format = llri::texture_format::RGBA8UNorm;
    desc.tiling = llri::texture_tiling::Optimal;

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

    SUBCASE("[Incorrect usage] desc.type was not a valid enum value")
    {
        desc.type = static_cast<llri::resource_type>(UINT_MAX);
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Incorrect usage] desc.usage was not a valid combination of resource_usage_flag_bits")
    {
        desc.usage = UINT_MAX;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.usage had the DenyShaderResource bit set but not the DepthStencilAttachment bit")
    {
        desc.type = llri::resource_type::Texture2D;
        desc.usage = llri::resource_usage_flag_bits::DenyShaderResource;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.usage had the DenyShaderResource bit set but had incompatible bits set. Compatible flags with DenyShaderResource are: DepthStencilAttachment, TransferSrc, TransferDst")
    {
        desc.type = llri::resource_type::Texture2D;
        desc.usage = llri::resource_usage_flag_bits::DenyShaderResource | llri::resource_usage_flag_bits::Sampled;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.memoryType was not a valid enum value")
    {
        desc.memoryType = static_cast<llri::resource_memory_type>(UINT_MAX);
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.memoryType was set to resource_memory_type::Upload and desc.usage contained resource_usage_flag_bits::ShaderWrite")
    {
        desc.usage = llri::resource_usage_flag_bits::ShaderWrite;
        desc.memoryType = llri::resource_memory_type::Upload;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.initialState was not a valid enum value")
    {
        desc.initialState = static_cast<llri::resource_state>(UINT_MAX);
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.initialState was set to Upload but also has resource_usage_flag_bits::ShaderWrite set")
    {
        desc.usage = llri::resource_usage_flag_bits::ShaderWrite;
        desc.initialState = llri::resource_state::Upload;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.initialState was set to ColorAttachment but desc.usage doesn't have the resource_usage_flag_bits::ColorAttachment bit set")
    {
        desc.type = llri::resource_type::Texture2D;
        desc.initialState = llri::resource_state::ColorAttachment;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.initialState was set to DepthStencilAttachment or DepthStencilAttachmentReadOnly, but desc.usage doesn't have the resource_usage_flag_bits::DepthStencilAttachment bit set")
    {
        desc.type = llri::resource_type::Texture2D;
        desc.initialState = llri::resource_state::DepthStencilAttachment;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);

        desc.initialState = llri::resource_state::DepthStencilAttachmentReadOnly;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.initialState was set to ShaderReadWrite but desc.usage doesn't have the resource_usage_flag_bits::ShaderWrite bit set")
    {
        desc.initialState = llri::resource_state::ShaderReadWrite;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.initialState was set to TransferSrc but desc.usage doesn't have the resource_usage_flag_bits::TransferSrc bit set")
    {
        desc.initialState = llri::resource_state::TransferSrc;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.initialState was set to TransferDst but desc.usage doesn't have the resource_usage_flag_bits::TransferDst bit set")
    {
        desc.initialState = llri::resource_state::TransferDst;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.memoryType was set to Local but desc.initialState was set to Upload")
    {
        desc.initialState = llri::resource_state::Upload;
        desc.memoryType = llri::resource_memory_type::Local;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.memoryType was set to Upload but desc.initialState was set to ShaderReadWrite")
    {
        desc.usage = llri::resource_usage_flag_bits::ShaderWrite;
        desc.initialState = llri::resource_state::ShaderReadWrite;
        desc.memoryType = llri::resource_memory_type::Upload;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] desc.width was not at least 1")
    {
        desc.type = llri::resource_type::Buffer;
        desc.width = 0;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("[Invalid usage] resource_type::Buffer incompatible resource usage flags")
    {
        constexpr llri::resource_usage_flags invalidUsage = llri::resource_usage_flag_bits::Sampled | llri::resource_usage_flag_bits::ColorAttachment | llri::resource_usage_flag_bits::DepthStencilAttachment | llri::resource_usage_flag_bits::DenyShaderResource;
        desc.type = llri::resource_type::Buffer;
        desc.usage = invalidUsage;
        CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
    }

    SUBCASE("resource_type::Texture1D/2D/3D")
    {
        std::set<llri::resource_state> invalidStates = { llri::resource_state::VertexBuffer, llri::resource_state::IndexBuffer, llri::resource_state::ConstantBuffer };
        std::set<llri::resource_type> types = { llri::resource_type::Texture1D, llri::resource_type::Texture2D, llri::resource_type::Texture3D };

        for (auto type : types)
        {
            desc.type = type;
            for (const auto state : invalidStates)
            {
                const std::string str = "[Invalid usage] incompatible initial state: " + llri::to_string(state);
                SUBCASE(str.c_str())
                {
                    desc.initialState = state;
                    CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
                }
            }

            desc.width = 1028;
            desc.height = 1;
            desc.depthOrArrayLayers = 1;
            desc.mipLevels = 1;
            desc.sampleCount = llri::texture_sample_count::Count1;
            desc.format = llri::texture_format::RGBA8UNorm;
            desc.tiling = llri::texture_tiling::Optimal;

            SUBCASE("[Invalid usage] desc.depthOrArrayLayers is not at least 1")
            {
                desc.depthOrArrayLayers = 0;
                CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Invalid usage] desc.mipLevels is not at least 1")
            {
                desc.mipLevels = 0;
                CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Invalid usage] desc.sampleCount is not a valid enum value")
            {
                desc.sampleCount = static_cast<llri::texture_sample_count>(UINT_MAX);
                CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Invalid usage] desc.usage had the ShaderWrite bit set but desc.sampleCount was not Count1")
            {
                desc.usage = llri::resource_usage_flag_bits::ShaderWrite;
                desc.sampleCount = llri::texture_sample_count::Count2;
                CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Invalid usage] desc.format was not a valid enum value")
            {
                desc.format = static_cast<llri::texture_format>(UINT_MAX);
                CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
            }

            SUBCASE("[Invalid usage] desc.tiling was not a valid enum value")
            {
                desc.tiling = static_cast<llri::texture_tiling>(UINT_MAX);
                CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
            }
        }
    }

    SUBCASE("resource_type::Texture1D")
    {
        desc.type = llri::resource_type::Texture1D;
        desc.initialState = llri::resource_state::General;
        desc.usage = {};
        desc.memoryType = llri::resource_memory_type::Local;
        desc.width = 1028;

        SUBCASE("[Invalid usage] desc.height was not 1")
        {
            desc.height = 1028;
            CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
        }
    }

    SUBCASE("resource_type::Texture2D")
    {
        desc.type = llri::resource_type::Texture2D;
        desc.width = 1028;

        SUBCASE("[Invalid usage] desc.type was Texture2D but desc.height was not at least 1")
        {
            desc.height = 0;
            CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
        }
    }

    SUBCASE("resource_type::Texture3D")
    {
        desc.type = llri::resource_type::Texture3D;
        desc.width = 1028;
        desc.height = 1028;
        desc.depthOrArrayLayers = 1;

        SUBCASE("[Invalid usage] desc.type was Texture3D but desc.height was not at least 1")
        {
            desc.height = 0;
            CHECK_EQ(device->createResource(desc, &failurePlaceholder), llri::result::ErrorInvalidUsage);
        }
    }

    SUBCASE("Fuzzy testing")
    {
        desc = {};

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

                                const std::set<uint32_t> possibleSizeValues = { 0, 1, 1024, UINT_MAX };
                                for (auto width : possibleSizeValues)
                                {
                                    desc.width = width;

                                    if (desc.type == llri::resource_type::Buffer)
                                    {
                                        desc.height = 1;
                                        desc.depthOrArrayLayers = 1;

                                        llri::Resource* resource = nullptr;
                                        llri::result result = device->createResource(desc, &resource);
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
    }

    instance->destroyDevice(device);
    llri::destroyInstance(instance);
}
