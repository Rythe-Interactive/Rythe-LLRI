/**
 * @file queue.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace LLRI_NAMESPACE
{
    result Queue::impl_submit(const submit_desc& desc)
    {
        HRESULT r;
        unsigned long index;
        _BitScanForward64(&index, desc.nodeMask);

        // add wait semaphores to queue
        for (size_t i = 0; i < desc.numWaitSemaphores; i++)
        {
            r = static_cast<ID3D12CommandQueue*>(m_ptrs[index])->Wait(static_cast<ID3D12Fence*>(desc.waitSemaphores[i]->m_ptr), desc.waitSemaphores[i]->m_counter);
            if (FAILED(r))
                return directx::mapHRESULT(r);
        }

        // submit
        std::vector<ID3D12CommandList*> lists(desc.numCommandLists);
        for (size_t i = 0; i < desc.numCommandLists; i++)
            lists[i] = static_cast<ID3D12CommandList*>(desc.commandLists[i]->m_ptr);

        static_cast<ID3D12CommandQueue*>(m_ptrs[index])->ExecuteCommandLists(desc.numCommandLists, lists.data());

        // add signal semaphores to queue
        for (size_t i = 0; i < desc.numSignalSemaphores; i++)
        {
            r = static_cast<ID3D12CommandQueue*>(m_ptrs[index])->Signal(static_cast<ID3D12Fence*>(desc.signalSemaphores[i]->m_ptr), ++desc.signalSemaphores[i]->m_counter); // NOTE: the convention is that we increase the counter upon signaling, all wait operations will use this counter without modifying it.
            if (FAILED(r))
                return directx::mapHRESULT(r);
        }

        // signal fence
        if (desc.fence)
        {
            r = static_cast<ID3D12CommandQueue*>(m_ptrs[index])->Signal(static_cast<ID3D12Fence*>(desc.fence->m_ptr), ++desc.fence->m_counter);
            if (FAILED(r))
                return directx::mapHRESULT(r);

            desc.fence->m_signaled = true;
        }

        return result::Success;
    }
}
