/**
 * @file command_group.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace LLRI_NAMESPACE
{
    result CommandGroup::impl_reset()
    {
        const auto r = static_cast<ID3D12CommandAllocator*>(m_ptr)->Reset();
        if (FAILED(r))
            return directx::mapHRESULT(r);

        for (auto* cmdList : m_cmdLists)
            cmdList->m_state = command_list_state::Empty;

        return result::Success;
    }
}
