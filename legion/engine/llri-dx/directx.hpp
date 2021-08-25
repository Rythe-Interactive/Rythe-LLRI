/**
 * @file directx.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp>
#include <graphics/directx/d3d12.h>
#include <dxgi1_6.h>

namespace LLRI_NAMESPACE
{
    namespace directx
    {
        using PFN_CREATE_DXGI_FACTORY2 = HRESULT(WINAPI*)(
            UINT Flags,
            REFIID riid,
            _COM_Outptr_ void** ppFactory
        );

        inline HMODULE dxgi = nullptr;
        inline PFN_CREATE_DXGI_FACTORY2 CreateDXGIFactory2 = nullptr;

        inline HMODULE d3d12 = nullptr;
        inline PFN_D3D12_CREATE_DEVICE D3D12CreateDevice = nullptr;
        inline PFN_D3D12_GET_DEBUG_INTERFACE D3D12GetDebugInterface = nullptr;

        inline void lazyInitializeDirectX()
        {
            if (d3d12 != nullptr && dxgi != nullptr)
                return;

            dxgi = LoadLibrary(L"dxgi");
            if (dxgi)
            {
                CreateDXGIFactory2 = (PFN_CREATE_DXGI_FACTORY2)GetProcAddress(dxgi, "CreateDXGIFactory2");
            }

            d3d12 = LoadLibrary(L"D3D12");
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
    }
}
