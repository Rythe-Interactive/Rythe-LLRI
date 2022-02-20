/**
 * @file device.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace llri
{
    result Device::impl_createCommandGroup(queue_type type, CommandGroup** cmdGroup)
    {
        auto* output = new CommandGroup();
        output->m_device = this;
        output->m_deviceFunctionTable = m_functionTable;
        output->m_validationCallbackMessenger = m_validationCallbackMessenger;
        output->m_type = type;

        ID3D12CommandAllocator* allocator;
        auto r = static_cast<ID3D12Device*>(m_ptr)->CreateCommandAllocator(detail::mapCommandGroupType(type), IID_PPV_ARGS(&allocator));
        if (FAILED(r))
        {
            destroyCommandGroup(output);
            return detail::mapHRESULT(r);
        }
        output->m_ptr = allocator;

        ID3D12CommandAllocator* indirectAllocator;
        r = static_cast<ID3D12Device*>(m_ptr)->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&indirectAllocator));
        if (FAILED(r))
        {
            destroyCommandGroup(output);
            return detail::mapHRESULT(r);
        }
        output->m_indirectPtr = indirectAllocator;

        *cmdGroup = output;
        return result::Success;
    }

    void Device::impl_destroyCommandGroup(CommandGroup* cmdGroup)
    {
        if (!cmdGroup)
            return;

        if (cmdGroup->m_ptr)
            static_cast<ID3D12CommandAllocator*>(cmdGroup->m_ptr)->Release();

        if (cmdGroup->m_indirectPtr)
            static_cast<ID3D12CommandAllocator*>(cmdGroup->m_indirectPtr)->Release();

        delete cmdGroup;
    }

    result Device::impl_createFence(fence_flags flags, Fence** fence)
    {
        ID3D12Fence* dx12Fence;
        const auto r = static_cast<ID3D12Device*>(m_ptr)->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&dx12Fence));
        if (FAILED(r))
            return detail::mapHRESULT(r);

        auto* output = new Fence();
        output->m_flags = flags;
        output->m_counter = 0;
        output->m_event = CreateEvent(nullptr, false, false, nullptr);
        output->m_ptr = dx12Fence;

        if ((flags & fence_flag_bits::Signaled) == fence_flag_bits::Signaled)
            output->m_signaled = true;

        *fence = output;
        return result::Success;
    }

    void Device::impl_destroyFence(Fence* fence)
    {
        if (fence->m_event)
            CloseHandle(fence->m_event);

        if (fence->m_ptr)
            static_cast<ID3D12Fence*>(fence->m_ptr)->Release();

        delete fence;
    }

    result Device::impl_waitFences(uint32_t numFences, Fence** fences, uint64_t timeout)
    {
        std::vector<void*> events;

        for (size_t i = 0; i < numFences; i++)
        {
            auto& fence = fences[i];
            auto* dx12Fence = static_cast<ID3D12Fence*>(fences[i]->m_ptr);

            if (dx12Fence->GetCompletedValue() < fence->m_counter)
            {
                const auto r = dx12Fence->SetEventOnCompletion(fence->m_counter, fence->m_event);
                if (FAILED(r))
                    return detail::mapHRESULT(r);

                events.push_back(fence->m_event);
            }
        }

        if (!events.empty())
        {
            const auto r = WaitForMultipleObjects(static_cast<DWORD>(events.size()), events.data(), true, static_cast<DWORD>(timeout)); // windows takes the timeout in ms so we can pass it directly

            if (r == WAIT_TIMEOUT)
                return result::Timeout;
        
            if (r == WAIT_FAILED)
                return result::ErrorUnknown;
        }

        for (size_t i = 0; i < numFences; i++)
            fences[i]->m_signaled = false;

        return result::Success;
    }

    result Device::impl_createSemaphore(Semaphore** semaphore)
    {
        // in the DX12 implementation, Semaphores are represented by DX12 Fences
        // DX12 Fences are more general purpose than some other implementations
        // but in LLRI this behaviour is split

        ID3D12Fence* dx12Fence;
        const auto r = static_cast<ID3D12Device*>(m_ptr)->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&dx12Fence));
        if (FAILED(r))
            return detail::mapHRESULT(r);

        auto* output = new Semaphore();
        output->m_ptr = dx12Fence;

        *semaphore = output;
        return result::Success;
    }

    void Device::impl_destroySemaphore(Semaphore* semaphore)
    {
        if (semaphore->m_ptr)
            static_cast<ID3D12Fence*>(semaphore->m_ptr)->Release();

        delete semaphore;
    }

    result Device::impl_createResource(const resource_desc& desc, Resource** resource)
    {
        const bool isTexture = desc.type != resource_type::Buffer;

        D3D12_RESOURCE_DESC dx12Desc;
        dx12Desc.Dimension = detail::mapResourceType(desc.type);
        dx12Desc.Alignment = 0;
        dx12Desc.Width = static_cast<UINT64>(desc.width);
        dx12Desc.Height = isTexture ? desc.height : 1;
        dx12Desc.DepthOrArraySize = isTexture ? static_cast<UINT16>(desc.depthOrArrayLayers) : 1;
        dx12Desc.MipLevels = isTexture ? static_cast<UINT16>(desc.mipLevels) : 1;
        dx12Desc.Format = isTexture ? detail::mapTextureFormat(desc.textureFormat) : DXGI_FORMAT_UNKNOWN;
        dx12Desc.SampleDesc = isTexture ? DXGI_SAMPLE_DESC{ static_cast<UINT>(desc.sampleCount), D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE } : DXGI_SAMPLE_DESC{ 1, 0 };
        dx12Desc.Layout = isTexture ? D3D12_TEXTURE_LAYOUT_UNKNOWN : D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        dx12Desc.Flags = detail::mapResourceUsage(desc.usage);

        const D3D12_RESOURCE_STATES initialState = detail::mapResourceState(desc.initialState);

        D3D12_HEAP_PROPERTIES heapProperties { detail::mapResourceMemoryType(desc.memoryType),
            D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN,
            desc.createNodeMask, desc.visibleNodeMask };

        D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;
        ID3D12Resource* dx12Resource = nullptr;

        const auto r = static_cast<ID3D12Device*>(m_ptr)->CreateCommittedResource(&heapProperties, flags, &dx12Desc, initialState, nullptr, IID_PPV_ARGS(&dx12Resource));
        if (FAILED(r))
            return detail::mapHRESULT(r);

        auto* output = new Resource();
        output->m_desc = desc;
        output->m_resource = dx12Resource;
        *resource = output;
        return result::Success;
    }

    void Device::impl_destroyResource(Resource* resource)
    {
        static_cast<ID3D12Resource*>(resource->m_resource)->Release();
        delete resource;
    }

    result Device::impl_createSwapchainEXT(const swapchain_desc_ext& desc, SwapchainEXT** swapchain)
    {
        DXGI_SWAP_CHAIN_DESC1 swapDesc{};
        swapDesc.Width = desc.textureExtent.width;
        swapDesc.Height = desc.textureExtent.height;
        swapDesc.Format = detail::mapTextureFormat(desc.textureFormat);
        swapDesc.SampleDesc = DXGI_SAMPLE_DESC { 1, 0 };
        swapDesc.BufferUsage = detail::mapResourceUsage(desc.textureUsage);
        swapDesc.BufferCount = desc.textureCount;
        swapDesc.Scaling = DXGI_SCALING_STRETCH;
        swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapDesc.Flags = 0;

        IDXGISwapChain1* dxSwapchain;
        const auto r = static_cast<IDXGIFactory2*>(m_instance->m_ptr)->CreateSwapChainForHwnd(
            static_cast<ID3D12CommandQueue*>(desc.queue->m_ptrs[0]), // todo nodemask?
            static_cast<HWND>(desc.surface->m_ptr),
            &swapDesc,
            nullptr,
            nullptr,
            &dxSwapchain);

        if (FAILED(r))
            return detail::mapHRESULT(r);

        SwapchainEXT* output = new SwapchainEXT();
        output->m_desc = desc;
        output->m_ptr = dxSwapchain;
        output->m_device = this;
        *swapchain = output;

        return result::Success;
    }

    void Device::impl_destroySwapchainEXT(SwapchainEXT* swapchain)
    {
        if (swapchain->m_ptr)
            static_cast<IDXGISwapChain1*>(swapchain->m_ptr)->Release();
        delete swapchain;
    }
}
