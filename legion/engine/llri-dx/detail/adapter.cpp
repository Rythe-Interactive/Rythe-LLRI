/**
 * @file adapter.cpp
 * Copyright (c) 2021 Leon Brands
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace llri
{
    adapter_info Adapter::impl_queryInfo() const
    {
        DXGI_ADAPTER_DESC1 desc;
        static_cast<IDXGIAdapter1*>(m_ptr)->GetDesc1(&desc);

        adapter_info info;
        info.vendorId = desc.VendorId;
        info.adapterId = desc.DeviceId;
        const auto description = std::string(reinterpret_cast<char*>(desc.Description), reinterpret_cast<char*>(desc.Description + 128));
        info.adapterName = description.substr(0, description.find_last_not_of(' '));

        if (desc.Flags == DXGI_ADAPTER_FLAG_REMOTE)
            info.adapterType = adapter_type::Virtual;
        else if (desc.DedicatedVideoMemory > 256000000) // safely assume that all GPUS with less than 256MB of VRAM are iGPUs
            info.adapterType = adapter_type::Discrete;
        else // video memory == 0 
            info.adapterType = adapter_type::Integrated;

        return info;
    }

    adapter_features Adapter::impl_queryFeatures() const
    {
        adapter_features features{};
        return features;
    }

    adapter_limits Adapter::impl_queryLimits() const
    {
        adapter_limits output{};
        return output;
    }

    bool Adapter::impl_queryExtensionSupport([[maybe_unused]] adapter_extension ext) const
    {
        return false;
    }

    result Adapter::impl_querySurfacePresentSupportEXT([[maybe_unused]] SurfaceEXT* surface, queue_type type, bool* support) const
    {
        switch(type)
        {
            case queue_type::Graphics:
                *support = true;
                break;
            case queue_type::Compute:
            case queue_type::Transfer:
                *support = false;
                break;
        }
        return result::Success;
    }

    result Adapter::impl_querySurfaceCapabilitiesEXT([[maybe_unused]] SurfaceEXT* surface, surface_capabilities_ext* capabilities) const
    {
        capabilities->minTextureCount = 2;
        capabilities->maxTextureCount = 16;

        capabilities->minExtent = { 1, 1 };
        capabilities->maxExtent = { 16384, 16384 };

        capabilities->formats = { format::RGBA8UNorm, format::BGRA8UNorm, format::RGBA16Float };

        capabilities->presentModes = { present_mode_ext::Immediate, present_mode_ext::Fifo };

        capabilities->usageBits = resource_usage_flag_bits::TransferSrc |
            resource_usage_flag_bits::TransferDst |
            resource_usage_flag_bits::ColorAttachment |
            resource_usage_flag_bits::Sampled;

        return result::Success;
    }
    
    uint8_t Adapter::impl_queryQueueCount(queue_type type) const
    {
        // DirectX doesn't have a limit on the number of created queues,
        // so we impose a reasonable arbitrary limit.
        // these values are taken from the queue limits on NVIDIA GPUs on the Vulkan implementation.
        switch(type)
        {
            case queue_type::Graphics:
            {
                return 16;
            }
            case queue_type::Compute:
            {
                return 8;
            }
            case queue_type::Transfer:
            {
                return 2;
            }
        }

        return 0;
    }

    std::unordered_map<format, format_properties> Adapter::impl_queryFormatProperties() const
    {
        std::unordered_map<format, format_properties> result;

        ID3D12Device* device;
        directx::D3D12CreateDevice(static_cast<IDXGIAdapter*>(m_ptr), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));

        CD3DX12FeatureSupport features;
        features.Init(device);

        for (uint8_t f = 0; f <= static_cast<uint8_t>(format::MaxEnum); f++)
        {
            const auto form = static_cast<format>(f);
            const DXGI_FORMAT dxFormat = directx::mapTextureFormat(form);

            // query support from DX12
            D3D12_FORMAT_SUPPORT1 sup1;
            D3D12_FORMAT_SUPPORT2 sup2;
            features.FormatSupport(dxFormat, sup1, sup2);

            // get support
            const bool supported = sup1 != D3D12_FORMAT_SUPPORT1_NONE;

            // get sample counts
            std::unordered_map<sample_count, bool> sampleCounts {
                { sample_count::Count1, false },
                { sample_count::Count2, false },
                { sample_count::Count4, false },
                { sample_count::Count8, false },
                { sample_count::Count16, false },
                { sample_count::Count32, false }
            };

            for (auto& pair : sampleCounts)
            {
                UINT count;
                features.MultisampleQualityLevels(dxFormat, static_cast<UINT>(pair.first), D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE, count);

                pair.second = count;
            }

            // get usage flags
            resource_usage_flags usageFlags = resource_usage_flag_bits::TransferSrc | resource_usage_flag_bits::TransferDst; // always supported

            if ((sup1 & D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE) == D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE)
                usageFlags |= resource_usage_flag_bits::Sampled;

            if ((sup1 & D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) == D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW)
                usageFlags |= resource_usage_flag_bits::ShaderWrite;

            if ((sup1 & D3D12_FORMAT_SUPPORT1_RENDER_TARGET) == D3D12_FORMAT_SUPPORT1_RENDER_TARGET)
                usageFlags |= resource_usage_flag_bits::ColorAttachment;

            if ((sup1 & D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL) == D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL)
            {
                usageFlags |= resource_usage_flag_bits::DepthStencilAttachment;
                usageFlags |= resource_usage_flag_bits::DenyShaderResource;
            }

            // get types
            const std::unordered_map<resource_type, bool> types {
                { resource_type::Buffer, false },
                { resource_type::Texture1D, (sup1 & D3D12_FORMAT_SUPPORT1_TEXTURE1D) == D3D12_FORMAT_SUPPORT1_TEXTURE1D },
                { resource_type::Texture2D, (sup1 & D3D12_FORMAT_SUPPORT1_TEXTURE2D) == D3D12_FORMAT_SUPPORT1_TEXTURE2D },
                { resource_type::Texture3D, (sup1 & D3D12_FORMAT_SUPPORT1_TEXTURE3D) == D3D12_FORMAT_SUPPORT1_TEXTURE3D }
            };

            // gather results
            result.insert({ form, format_properties { 
                supported,
                types,
                usageFlags,
                sampleCounts
            } });
        }

        device->Release();
        return result;
    }
}
