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
        // TODO: Handle node mask
        // TODO: Handle inheritance/indirect
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
                            directx::mapResourceState(barrier.trans.oldState),
                            directx::mapResourceState(barrier.trans.newState)
                        };
                        dx12Barriers.push_back(dx12Barrier);
                    }
                    else
                    {
                        const auto desc = barrier.trans.resource->getDesc();
                        const size_t arrayLayers = desc.type == resource_type::Texture3D ? 1 : desc.depthOrArrayLayers;
                        for (size_t a = barrier.trans.subresourceRange.baseArrayLayer; a < barrier.trans.subresourceRange.baseArrayLayer + barrier.trans.subresourceRange.numArrayLayers; a++)
                        {
                            for (size_t m = barrier.trans.subresourceRange.baseMipLevel; m < barrier.trans.subresourceRange.baseMipLevel + barrier.trans.subresourceRange.numMipLevels; m++)
                            {
                                D3D12_RESOURCE_BARRIER dx12Barrier{};
                                dx12Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                                dx12Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                                dx12Barrier.Transition = D3D12_RESOURCE_TRANSITION_BARRIER {
                                    static_cast<ID3D12Resource*>(barrier.rw.resource->m_resource),
                                    D3D12CalcSubresource(m, a, 0, desc.mipLevels, arrayLayers),
                                    directx::mapResourceState(barrier.trans.oldState),
                                    directx::mapResourceState(barrier.trans.newState)
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
