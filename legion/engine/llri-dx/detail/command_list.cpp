/**
 * @file command_list.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace llri
{
    result CommandList::impl_begin(const command_list_begin_desc& desc)
    {
        //TODO: Handle node mask
        //TODO: Handle inheritance/indirect
        const auto r = static_cast<ID3D12GraphicsCommandList*>(m_ptr)->Reset(static_cast<ID3D12CommandAllocator*>(m_group->m_ptr), nullptr);
        if (FAILED(r))
            return directx::mapHRESULT(r);

        m_state = command_list_state::Recording;
        return result::Success;
    }

    result CommandList::impl_end()
    {
        const auto r = static_cast<ID3D12GraphicsCommandList*>(m_ptr)->Close();
         if (FAILED(r))
             return directx::mapHRESULT(r);

        m_state = command_list_state::Ready;
        return result::Success;
    }
}
