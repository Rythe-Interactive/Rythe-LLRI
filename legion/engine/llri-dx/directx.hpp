/**
 * @file directx.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp>
#define INITGUID
#include <graphics/directx/d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>

namespace LLRI_NAMESPACE
{
    namespace directx
    {
        using PFN_CREATE_DXGI_FACTORY2 = HRESULT(WINAPI*)(
            UINT Flags,
            REFIID riid,
            _COM_Outptr_ void** ppFactory
        );

        using PFN_DXGI_GET_DEBUG_INTERFACE1 = HRESULT(WINAPI*)(
            UINT Flags,
            REFIID riid,
            _COM_Outptr_ void** ppFactory
        );

        inline HMODULE dxgi = nullptr;
        inline PFN_CREATE_DXGI_FACTORY2 CreateDXGIFactory2 = nullptr;
        inline PFN_DXGI_GET_DEBUG_INTERFACE1 DXGIGetDebugInterface1 = nullptr;

        inline HMODULE d3d12 = nullptr;
        inline PFN_D3D12_CREATE_DEVICE D3D12CreateDevice = nullptr;
        inline PFN_D3D12_GET_DEBUG_INTERFACE D3D12GetDebugInterface = nullptr;

        inline void lazyInitializeDirectX()
        {
            if (d3d12 != nullptr && dxgi != nullptr)
                return;

            dxgi = LoadLibrary("dxgi");
            if (dxgi)
            {
                CreateDXGIFactory2 = (PFN_CREATE_DXGI_FACTORY2)GetProcAddress(dxgi, "CreateDXGIFactory2");
                DXGIGetDebugInterface1 = (PFN_DXGI_GET_DEBUG_INTERFACE1)GetProcAddress(dxgi, "DXGIGetDebugInterface1");
            }

            d3d12 = LoadLibrary("D3D12");
            if (d3d12)
            {
                D3D12CreateDevice = (PFN_D3D12_CREATE_DEVICE)GetProcAddress(d3d12, "D3D12CreateDevice");
                D3D12GetDebugInterface = (PFN_D3D12_GET_DEBUG_INTERFACE)GetProcAddress(d3d12, "D3D12GetDebugInterface");
            }
        }

        /**
         * @brief Function that maps an HRESULT to an llri::result.
        */
        constexpr result mapHRESULT(const HRESULT value)
        {
            switch (value)
            {
                case S_OK :
                    return result::Success;
                case DXGI_ERROR_WAIT_TIMEOUT :
                    return result::Timeout;
                case DXGI_ERROR_DEVICE_HUNG :
                    return result::ErrorDeviceHung;
                case DXGI_ERROR_DEVICE_REMOVED :
                    return result::ErrorDeviceRemoved;
                case DXGI_ERROR_DEVICE_RESET :
                    return result::ErrorDeviceLost;
                case DXGI_ERROR_DRIVER_INTERNAL_ERROR :
                    return result::ErrorDriverFailure;
                case DXGI_ERROR_UNSUPPORTED :
                    return result::ErrorFeatureNotSupported;
                case D3D12_ERROR_ADAPTER_NOT_FOUND:
                    break;
                case D3D12_ERROR_DRIVER_VERSION_MISMATCH:
                    return result::ErrorIncompatibleDriver;
                case DXGI_ERROR_INVALID_CALL:
                    return result::ErrorInvalidUsage;
                case DXGI_ERROR_WAS_STILL_DRAWING:
                    break;
                case E_FAIL:
                    break;
                case E_NOINTERFACE:
                    return result::ErrorExtensionNotSupported;
                case E_INVALIDARG:
                    return result::ErrorInvalidUsage;
                case E_OUTOFMEMORY:
                    return result::ErrorOutOfHostMemory;
                case E_NOTIMPL:
                    return result::ErrorFeatureNotSupported;
                case S_FALSE:
                    return result::NotReady;
            }

            return result::ErrorUnknown;
        }

        constexpr D3D12_COMMAND_LIST_TYPE mapCommandGroupType(queue_type type)
        {
            switch (type)
            {
                case queue_type::Graphics:
                    return D3D12_COMMAND_LIST_TYPE_DIRECT;
                case queue_type::Compute:
                    return D3D12_COMMAND_LIST_TYPE_COMPUTE;
                case queue_type::Transfer:
                    return D3D12_COMMAND_LIST_TYPE_COPY;
            }

            throw;
        }

        constexpr D3D12_RESOURCE_DIMENSION mapResourceType(resource_type type)
        {
            switch(type)
            {
                case resource_type::Buffer:
                    return D3D12_RESOURCE_DIMENSION_BUFFER;
                case resource_type::Texture1D:
                    return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
                case resource_type::Texture2D:
                    return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
                case resource_type::Texture3D:
                    return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
            }

            throw;
        }

        constexpr DXGI_FORMAT mapTextureFormat(texture_format format)
        {
            switch(format)
            {
                case texture_format::Undefined:
                    return DXGI_FORMAT_UNKNOWN;
                case texture_format::R8UNorm:
                    return DXGI_FORMAT_R8_UNORM;
                case texture_format::R8Norm:
                    return DXGI_FORMAT_R8_SNORM;
                case texture_format::R8UInt:
                    return DXGI_FORMAT_R8_UINT;
                case texture_format::R8Int:
                    return DXGI_FORMAT_R8_SINT;
                case texture_format::RG8UNorm:
                    return DXGI_FORMAT_R8G8_UNORM;
                case texture_format::RG8Norm:
                    return DXGI_FORMAT_R8G8_SNORM;
                case texture_format::RG8UInt:
                    return DXGI_FORMAT_R8G8_UINT;
                case texture_format::RG8Int:
                    return DXGI_FORMAT_R8G8_SINT;
                case texture_format::RGBA8UNorm:
                    return DXGI_FORMAT_R8G8B8A8_UNORM;
                case texture_format::RGBA8Norm:
                    return DXGI_FORMAT_R8G8B8A8_SNORM;
                case texture_format::RGBA8UInt:
                    return DXGI_FORMAT_R8G8B8A8_UINT;
                case texture_format::RGBA8Int:
                    return DXGI_FORMAT_R8G8B8A8_SINT;
                case texture_format::RGBA8sRGB:
                    return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                case texture_format::BGRA8UNorm:
                    return DXGI_FORMAT_B8G8R8A8_UNORM;
                case texture_format::BGRA8sRGB:
                    return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
                case texture_format::RGB10A2UNorm:
                    return DXGI_FORMAT_R10G10B10A2_UNORM;
                case texture_format::RGB10A2UInt:
                    return DXGI_FORMAT_R10G10B10A2_UINT;
                case texture_format::R16UNorm:
                    return DXGI_FORMAT_R16_UNORM;
                case texture_format::R16Norm:
                    return DXGI_FORMAT_R16_SNORM;
                case texture_format::R16UInt:
                    return DXGI_FORMAT_R16_UINT;
                case texture_format::R16Int:
                    return DXGI_FORMAT_R16_SINT;
                case texture_format::R16Float:
                    return DXGI_FORMAT_R16_FLOAT;
                case texture_format::RG16UNorm:
                    return DXGI_FORMAT_R16G16_UNORM;
                case texture_format::RG16Norm:
                    return DXGI_FORMAT_R16G16_SNORM;
                case texture_format::RG16UInt:
                    return DXGI_FORMAT_R16G16_UINT;
                case texture_format::RG16Int:
                    return DXGI_FORMAT_R16G16_SINT;
                case texture_format::RG16Float:
                    return DXGI_FORMAT_R16G16_FLOAT;
                case texture_format::RGBA16UNorm:
                    return DXGI_FORMAT_R16G16B16A16_UNORM;
                case texture_format::RGBA16Norm:
                    return DXGI_FORMAT_R16G16B16A16_SNORM;
                case texture_format::RGBA16UInt:
                    return DXGI_FORMAT_R16G16B16A16_UINT;
                case texture_format::RGBA16Int:
                    return DXGI_FORMAT_R16G16B16A16_SINT;
                case texture_format::RGBA16Float:
                    return DXGI_FORMAT_R16G16B16A16_FLOAT;
                case texture_format::R32UInt:
                    return DXGI_FORMAT_R32_UINT;
                case texture_format::R32Int:
                    return DXGI_FORMAT_R32_SINT;
                case texture_format::R32Float:
                    return DXGI_FORMAT_R32_FLOAT;
                case texture_format::RG32UInt:
                    return DXGI_FORMAT_R32G32_UINT;
                case texture_format::RG32Int:
                    return DXGI_FORMAT_R32G32_SINT;
                case texture_format::RG32Float:
                    return DXGI_FORMAT_R32G32_FLOAT;
                case texture_format::RGB32UInt:
                    return DXGI_FORMAT_R32G32B32_UINT;
                case texture_format::RGB32Int:
                    return DXGI_FORMAT_R32G32B32_SINT;
                case texture_format::RGB32Float:
                    return DXGI_FORMAT_R32G32B32_FLOAT;
                case texture_format::RGBA32UInt:
                    return DXGI_FORMAT_R32G32B32A32_UINT;
                case texture_format::RGBA32Int:
                    return DXGI_FORMAT_R32G32B32A32_SINT;
                case texture_format::RGBA32Float:
                    return DXGI_FORMAT_R32G32B32A32_FLOAT;
                case texture_format::D16UNorm:
                    return DXGI_FORMAT_D16_UNORM;
                case texture_format::D24UNormS8UInt:
                    return DXGI_FORMAT_D24_UNORM_S8_UINT;
                case texture_format::D32Float:
                    return DXGI_FORMAT_D32_FLOAT;
                case texture_format::D32FloatS8X24UInt:
                    return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            }

            throw;
        }

        constexpr D3D12_TEXTURE_LAYOUT mapTextureTiling(texture_tiling tiling)
        {
            switch (tiling)
            {
                case texture_tiling::Optimal:
                    return D3D12_TEXTURE_LAYOUT_UNKNOWN;
                case texture_tiling::Linear:
                    return D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            }

            throw;
        }

        constexpr D3D12_RESOURCE_FLAGS mapResourceUsage(resource_usage_flags flags)
        {
            D3D12_RESOURCE_FLAGS output = D3D12_RESOURCE_FLAG_NONE;

            if (flags.contains(resource_usage_flag_bits::ShaderWrite))
                output |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            if (flags.contains(resource_usage_flag_bits::ColorAttachment))
                output |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
            if (flags.contains(resource_usage_flag_bits::DepthStencilAttachment))
                output |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            if (flags.contains(resource_usage_flag_bits::DenyShaderResource))
                output |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

            return output;
        }

        constexpr D3D12_RESOURCE_STATES mapResourceState(resource_state state)
        {
            switch(state)
            {
                case resource_state::General:
                    return D3D12_RESOURCE_STATE_COMMON;
                case resource_state::Upload:
                    return D3D12_RESOURCE_STATE_GENERIC_READ;
                case resource_state::ColorAttachment:
                    return D3D12_RESOURCE_STATE_RENDER_TARGET;
                case resource_state::DepthStencilAttachment:
                    return D3D12_RESOURCE_STATE_DEPTH_WRITE;
                case resource_state::DepthStencilAttachmentReadOnly:
                    return D3D12_RESOURCE_STATE_DEPTH_READ;
                case resource_state::ShaderReadOnly:
                    return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
                case resource_state::ShaderReadWrite:
                    return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
                case resource_state::TransferSrc:
                    return D3D12_RESOURCE_STATE_COPY_SOURCE;
                case resource_state::TransferDst:
                    return D3D12_RESOURCE_STATE_COPY_DEST;
                case resource_state::VertexBuffer:
                    return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
                case resource_state::IndexBuffer:
                    return D3D12_RESOURCE_STATE_INDEX_BUFFER;
                case resource_state::ConstantBuffer:
                    return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
            }

            throw;
        }

        constexpr D3D12_HEAP_TYPE mapResourceMemoryType(resource_memory_type memory)
        {
            switch(memory)
            {
                case resource_memory_type::Local:
                    return D3D12_HEAP_TYPE_DEFAULT;
                case resource_memory_type::Upload:
                    return D3D12_HEAP_TYPE_UPLOAD;
                case resource_memory_type::Read:
                    return D3D12_HEAP_TYPE_READBACK;
            }

            throw;
        }
    }
}
