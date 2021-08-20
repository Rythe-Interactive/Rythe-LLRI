/**
 * @file command_group.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace LLRI_NAMESPACE
{
    namespace internal
    {
        result mapHRESULT(HRESULT value);
    }

    result CommandGroup::impl_reset()
    {
        return internal::mapHRESULT(static_cast<ID3D12CommandAllocator*>(m_ptr)->Reset());
    }
}
