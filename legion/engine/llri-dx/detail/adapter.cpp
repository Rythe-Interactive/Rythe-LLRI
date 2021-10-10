/**
 * @file adapter.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace LLRI_NAMESPACE
{
    result Adapter::impl_queryInfo(adapter_info* info) const
    {
        DXGI_ADAPTER_DESC1 desc;
        static_cast<IDXGIAdapter1*>(m_ptr)->GetDesc1(&desc);

        adapter_info result;
        result.vendorId = desc.VendorId;
        result.adapterId = desc.DeviceId;
        const auto description = std::string(desc.Description, desc.Description + 128);
        result.adapterName = description.substr(0, description.find_last_not_of(' '));

        if (desc.Flags == DXGI_ADAPTER_FLAG_REMOTE)
            result.adapterType = adapter_type::Virtual;
        else if (desc.DedicatedVideoMemory > 256000000) //safely assume that all GPUS with less than 256MB of VRAM are iGPUs
            result.adapterType = adapter_type::Discrete;
        else // video memory == 0 
            result.adapterType = adapter_type::Integrated;

        *info = result;
        return result::Success;
    }

    result Adapter::impl_queryFeatures(adapter_features* features) const
    {
        HRESULT level12_0 = directx::D3D12CreateDevice(static_cast<IDXGIAdapter*>(m_ptr), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
        if (level12_0 == E_FAIL)
            return result::ErrorIncompatibleDriver;
        else if (FAILED(level12_0)) //no matter what reason, LEVEL_12_0 should always be supported for DX12
            return directx::mapHRESULT(level12_0);

        //Level 1 and 2 can more easily grant us guarantees about feature support
        HRESULT level12_1 = directx::D3D12CreateDevice(static_cast<IDXGIAdapter*>(m_ptr), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);
        HRESULT level12_2 = directx::D3D12CreateDevice(static_cast<IDXGIAdapter*>(m_ptr), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr);

        adapter_features output;

        //Set all the information in a structured way here

        *features = output;
        return result::Success;
    }

    result Adapter::impl_queryExtensionSupport(adapter_extension_type type, bool* supported) const
    {
        switch (type)
        {
            default:
                break;
        }

        return result::Success;
    }

    result Adapter::impl_queryQueueCount(queue_type type, uint8_t* count) const
    {
        //DirectX doesn't have a limit on the number of created queues,
        //so we impose a reasonable arbitrary limit.
        //these values are taken from the queue limits on NVIDIA GPUs on the Vulkan implementation.
        switch(type)
        {
            case queue_type::Graphics:
            {
                *count = 16;
                break;
            }
            case queue_type::Compute:
            {
                *count = 8;
                break;
            }
            case queue_type::Transfer:
            {
                *count = 2;
                break;
            }
        }
;
        return result::Success;
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

            D3D12_FORMAT_SUPPORT1 sup1;
            D3D12_FORMAT_SUPPORT2 sup2;
            features.FormatSupport(dxFormat, sup1, sup2);

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

            result.insert({ form, format_properties { 
                sup1 != D3D12_FORMAT_SUPPORT1_NONE,
                usageFlags,
                true,
                sampleCounts
            } });
        }

        device->Release();
        return result;
    }
}
