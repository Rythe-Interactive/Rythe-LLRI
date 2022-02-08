/**
 * @file queue.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace llri
{
    result Queue::impl_submit(const submit_desc& desc)
    {
        HRESULT r;
        unsigned long index;

        if (desc.nodeMask != 0)
            _BitScanForward64(&index, desc.nodeMask);
        else
            index = 0;

        // add wait semaphores to queue
        for (size_t i = 0; i < desc.numWaitSemaphores; i++)
        {
            r = static_cast<ID3D12CommandQueue*>(m_ptrs[index])->Wait(static_cast<ID3D12Fence*>(desc.waitSemaphores[i]->m_ptr), desc.waitSemaphores[i]->m_counter);
            if (FAILED(r))
                return detail::mapHRESULT(r);
        }

        // submit
        std::vector<ID3D12CommandList*> lists(desc.numCommandLists);
        for (size_t i = 0; i < desc.numCommandLists; i++)
            lists[i] = static_cast<ID3D12CommandList*>(desc.commandLists[i]->m_ptr);

        static_cast<ID3D12CommandQueue*>(m_ptrs[index])->ExecuteCommandLists(desc.numCommandLists, lists.data());

        // add signal semaphores to queue
        for (size_t i = 0; i < desc.numSignalSemaphores; i++)
        {
            // NOTE: the convention is that we increase the counter upon signaling, all wait operations will use this counter without modifying it.
            r = static_cast<ID3D12CommandQueue*>(m_ptrs[index])->Signal(static_cast<ID3D12Fence*>(desc.signalSemaphores[i]->m_ptr), ++desc.signalSemaphores[i]->m_counter);
            if (FAILED(r))
                return detail::mapHRESULT(r);
        }

        // signal fence
        if (desc.fence)
        {
            r = static_cast<ID3D12CommandQueue*>(m_ptrs[index])->Signal(static_cast<ID3D12Fence*>(desc.fence->m_ptr), ++desc.fence->m_counter);
            if (FAILED(r))
                return detail::mapHRESULT(r);

            desc.fence->m_signaled = true;
        }

        return result::Success;
    }

    result Queue::impl_waitIdle()
    {
        for (size_t i = 0; i < m_ptrs.size(); i++)
        {
            auto* queue = static_cast<ID3D12CommandQueue*>(m_ptrs[i]);
            auto* fence = m_fences[i];

            // NOTE: the convention is that we increase the counter upon signaling, all wait operations will use this counter without modifying it.
            queue->Signal(static_cast<ID3D12Fence*>(fence->m_ptr), ++fence->m_counter);
            fence->m_signaled = true;
        }

        return m_device->waitFences(static_cast<uint32_t>(m_fences.size()), m_fences.data(), LLRI_TIMEOUT_MAX);
    }
}
