#pragma once
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
    }
}