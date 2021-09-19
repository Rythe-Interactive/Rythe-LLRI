/**
 * @file device.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace LLRI_NAMESPACE
{
    result Device::impl_createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup)
    {
        auto* output = new CommandGroup();
        output->m_device = this;
        output->m_deviceFunctionTable = m_functionTable;
        output->m_validationCallback = m_validationCallback;
        output->m_validationCallbackMessenger = m_validationCallbackMessenger;
        output->m_maxCount = desc.count;
        output->m_type = desc.type;

        ID3D12CommandAllocator* allocator;
        auto r = static_cast<ID3D12Device*>(m_ptr)->CreateCommandAllocator(directx::mapCommandGroupType(desc.type), IID_PPV_ARGS(&allocator));
        if (FAILED(r))
        {
            destroyCommandGroup(output);
            return directx::mapHRESULT(r);
        }
        output->m_ptr = allocator;

        ID3D12CommandAllocator* indirectAllocator;
        r = static_cast<ID3D12Device*>(m_ptr)->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&indirectAllocator));
        if (FAILED(r))
        {
            destroyCommandGroup(output);
            return directx::mapHRESULT(r);
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
        // fence flag signaled ignored, waiting on the default fence is valid regardless

        ID3D12Fence* dx12Fence;
        const auto r = static_cast<ID3D12Device*>(m_ptr)->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&dx12Fence));
        if (FAILED(r))
            return directx::mapHRESULT(r);

        auto* output = new Fence();
        output->m_counter = 0;
        output->m_event = CreateEvent(nullptr, false, false, nullptr);
        output->m_ptr = dx12Fence;

        *fence = output;
        return result::Success;
    }

    void Device::impl_destroyFence(Fence* fence)
    {
        if (fence->m_event)
            CloseHandle(fence->m_event);

        if (fence->m_ptr)
            static_cast<ID3D12Fence*>(fence->m_ptr)->Release();
    }

    result Device::impl_waitFences(uint32_t numFences, Fence** fences, uint64_t timeout)
    {
        for (size_t i = 0; i < numFences; i++)
        {
            auto& fence = fences[i];
            auto* dx12Fence = static_cast<ID3D12Fence*>(fences[i]->m_ptr);

            if (dx12Fence->GetCompletedValue() < fence->m_counter)
		    {
			    auto r = dx12Fence->SetEventOnCompletion(fence->m_counter, fence->m_event);
                if (FAILED(r))
                    return directx::mapHRESULT(r);

                r = WaitForSingleObject(fence->m_event, timeout); // windows takes the timeout in ms so we can pass it directly

                if (r == WAIT_TIMEOUT)
                    return result::Timeout;

                if (r == WAIT_FAILED)
                    return result::ErrorUnknown;
		    }
        }

        return result::Success;
    }
}
