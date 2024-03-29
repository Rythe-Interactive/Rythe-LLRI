/**
 * @file command_list.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace llri
{
    result CommandList::impl_begin([[maybe_unused]] const command_list_begin_desc& desc)
    {
        // TODO: Handle node mask
        // TODO: Handle inheritance/indirect
        const auto r = static_cast<ID3D12GraphicsCommandList*>(m_ptr)->Reset(static_cast<ID3D12CommandAllocator*>(m_group->m_ptr), nullptr);
        if (FAILED(r))
            return detail::mapHRESULT(r);

        m_state = command_list_state::Recording;
        return result::Success;
    }

    result CommandList::impl_end()
    {
        const auto r = static_cast<ID3D12GraphicsCommandList*>(m_ptr)->Close();
         if (FAILED(r))
             return detail::mapHRESULT(r);

        m_state = command_list_state::Ready;
        return result::Success;
    }

    result CommandList::impl_resourceBarrier(uint32_t numBarriers, const resource_barrier* barriers)
    {
        std::vector<D3D12_RESOURCE_BARRIER> dx12Barriers;

        for (size_t i = 0; i < numBarriers; i++)
        {
            auto& barrier = barriers[i];

            switch(barrier.type)
            {
                case resource_barrier_type::ReadWrite:
                {
                    D3D12_RESOURCE_BARRIER dx12Barrier{};
                    dx12Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                    dx12Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
                    dx12Barrier.UAV = D3D12_RESOURCE_UAV_BARRIER { static_cast<ID3D12Resource*>(barrier.rw.resource->m_resource) };
                    dx12Barriers.push_back(dx12Barrier);
                    break;
                }
                case resource_barrier_type::Transition:
                {
                    if (barrier.trans.subresourceRange == texture_subresource_range::all())
                    {
                        D3D12_RESOURCE_BARRIER dx12Barrier{};
                        dx12Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                        dx12Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                        dx12Barrier.Transition = D3D12_RESOURCE_TRANSITION_BARRIER {
                            static_cast<ID3D12Resource*>(barrier.rw.resource->m_resource),
                            D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                            detail::mapResourceState(barrier.trans.oldState),
                            detail::mapResourceState(barrier.trans.newState)
                        };
                        dx12Barriers.push_back(dx12Barrier);
                    }
                    else
                    {
                        const auto desc = barrier.trans.resource->getDesc();
                        const UINT arrayLayers = desc.type == resource_type::Texture3D ? 1u : desc.depthOrArrayLayers;
                        for (UINT a = barrier.trans.subresourceRange.baseArrayLayer; a < barrier.trans.subresourceRange.baseArrayLayer + barrier.trans.subresourceRange.numArrayLayers; a++)
                        {
                            for (UINT m = barrier.trans.subresourceRange.baseMipLevel; m < barrier.trans.subresourceRange.baseMipLevel + barrier.trans.subresourceRange.numMipLevels; m++)
                            {
                                D3D12_RESOURCE_BARRIER dx12Barrier{};
                                dx12Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                                dx12Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                                dx12Barrier.Transition = D3D12_RESOURCE_TRANSITION_BARRIER {
                                    static_cast<ID3D12Resource*>(barrier.rw.resource->m_resource),
                                    D3D12CalcSubresource(m, a, 0, desc.mipLevels, arrayLayers),
                                    detail::mapResourceState(barrier.trans.oldState),
                                    detail::mapResourceState(barrier.trans.newState)
                                };
                                dx12Barriers.push_back(dx12Barrier);
                            }
                        }
                    }
                }
            }
        }

        static_cast<ID3D12GraphicsCommandList*>(m_ptr)->ResourceBarrier(numBarriers, dx12Barriers.data());
        return result::Success;
    }
}
