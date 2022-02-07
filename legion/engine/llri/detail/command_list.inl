/**
 * @file command_list.inl
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    inline std::string to_string(command_list_usage usage)
    {
        switch(usage)
        {
            case command_list_usage::Direct:
                return "Direct";
            case command_list_usage::Indirect:
                return "Indirect";
        }

        return "Invalid command_list_usage value";
    }

    inline std::string to_string(command_list_state state)
    {
        switch(state)
        {
            case command_list_state::Empty:
                return "Empty";
            case command_list_state::Recording:
                return "Recording";
            case command_list_state::Ready:
                return "Ready";
        }

        return "Invalid command_list_state value";
    }

    inline command_list_alloc_desc CommandList::getDesc() const
    {
        return m_desc;
    }

    inline CommandList::native_command_list* CommandList::getNative() const
    {
        return m_ptr;
    }

    inline result CommandList::begin(const command_list_begin_desc& desc)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(getState() == command_list_state::Empty, result::ErrorInvalidState)
        
        LLRI_DETAIL_VALIDATION_REQUIRE(m_group->m_currentlyRecording == nullptr, result::ErrorOccupied)

#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        m_group->m_currentlyRecording = this;
#endif

        LLRI_DETAIL_CALL_IMPL(impl_begin(desc), m_validationCallbackMessenger)
    }

    inline result CommandList::end()
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(getState() == command_list_state::Recording, result::ErrorInvalidState)

#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        m_group->m_currentlyRecording = nullptr;
#endif

        LLRI_DETAIL_CALL_IMPL(impl_end(), m_validationCallbackMessenger)
    }

    template<typename Func, typename ...Args>
    result CommandList::record(const command_list_begin_desc& desc, Func&& function, Args&&... args)
    {
        const result r = begin(desc);
        if (r != result::Success)
            return r;

        std::invoke(std::forward<Func>(function), std::forward<Args>(args)...);

        return end();
    }
    
    inline result CommandList::resourceBarrier(uint32_t numBarriers, const resource_barrier* barriers)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(getState() == command_list_state::Recording, llri::result::ErrorInvalidState)
        
        LLRI_DETAIL_VALIDATION_REQUIRE(numBarriers > 0, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(barriers != nullptr, result::ErrorInvalidUsage)
        
#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        for (size_t i = 0; i < numBarriers; i++)
        {
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].type <= resource_barrier_type::MaxEnum, i, result::ErrorInvalidUsage)
            
            switch (barriers[i].type)
            {
                case resource_barrier_type::ReadWrite:
                {
                    LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].rw.resource != nullptr, i, result::ErrorInvalidUsage)
                    break;
                }
                case resource_barrier_type::Transition:
                {
                    LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource != nullptr, i, result::ErrorInvalidUsage)
                    LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.oldState != barriers[i].trans.newState, i, result::ErrorInvalidUsage)
					
					const auto resourceDesc = barriers[i].trans.resource->getDesc();
					
					// validate subresource range
					if (resourceDesc.type != resource_type::Buffer && barriers[i].trans.subresourceRange != texture_subresource_range::all())
					{
						LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.subresourceRange.baseMipLevel < resourceDesc.mipLevels, i, result::ErrorInvalidUsage)
						
						LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.subresourceRange.numMipLevels > 0, i, result::ErrorInvalidUsage)
						
						LLRI_DETAIL_VALIDATION_REQUIRE_ITER((barriers[i].trans.subresourceRange.baseMipLevel + barriers[i].trans.subresourceRange.numMipLevels) <= resourceDesc.mipLevels, i, result::ErrorInvalidUsage)
						
						LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.subresourceRange.baseArrayLayer < resourceDesc.depthOrArrayLayers, i, result::ErrorInvalidUsage)
						
						LLRI_DETAIL_VALIDATION_REQUIRE_ITER((resourceDesc.type != resource_type::Texture3D) || (barriers[i].trans.subresourceRange.baseArrayLayer == 0), i, result::ErrorInvalidUsage)
						
						if (resourceDesc.type == resource_type::Texture3D)
						{
							LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.subresourceRange.numArrayLayers == 1, i, result::ErrorInvalidUsage)
						}
						else
						{
							LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.subresourceRange.numArrayLayers > 0, i, result::ErrorInvalidUsage)
							
							LLRI_DETAIL_VALIDATION_REQUIRE_ITER((barriers[i].trans.subresourceRange.baseArrayLayer + barriers[i].trans.subresourceRange.numArrayLayers) <= resourceDesc.depthOrArrayLayers, i, result::ErrorInvalidUsage)
						}
					}
					
					// validate new state correctness
                    LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.newState <= resource_state::MaxEnum, i, result::ErrorInvalidUsage)
                    
                    switch(barriers[i].trans.newState)
                    {
                        case resource_state::General:
                        {
							// no requirements
                            break;
                        }
                        case resource_state::Upload:
                        {
                            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource->getDesc().memoryType == memory_type::Upload, i, result::ErrorInvalidState)
                            break;
                        }
                        case resource_state::ColorAttachment:
                        {
							LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource->getDesc().
								usage.contains(resource_usage_flag_bits::ColorAttachment), i, result::ErrorInvalidState)
                            break;
                        }
                        case resource_state::DepthStencilAttachment:
                        {
                            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource->getDesc().
								usage.contains(resource_usage_flag_bits::DepthStencilAttachment), i, result::ErrorInvalidState)
                            break;
                        }
                        case resource_state::DepthStencilAttachmentReadOnly:
                        {
                            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource->getDesc().
								usage.contains(resource_usage_flag_bits::DepthStencilAttachment), i, result::ErrorInvalidState)
                            break;
                        }
                        case resource_state::ShaderReadOnly:
                        {
							if (barriers[i].trans.resource->getDesc().type != resource_type::Buffer)
								LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource->getDesc().
									usage.contains(resource_usage_flag_bits::Sampled), i, result::ErrorInvalidState)
                            break;
                        }
                        case resource_state::ShaderReadWrite:
                        {
							LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource->getDesc().
								usage.contains(resource_usage_flag_bits::ShaderWrite), i, result::ErrorInvalidState)
                            break;
                        }
                        case resource_state::TransferSrc:
                        {
							LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource->getDesc().
								usage.contains(resource_usage_flag_bits::TransferSrc), i, result::ErrorInvalidState)
                            break;
                        }
                        case resource_state::TransferDst:
                        {
							LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource->getDesc().
								usage.contains(resource_usage_flag_bits::TransferDst), i, result::ErrorInvalidState)
                            break;
                        }
                        case resource_state::VertexBuffer:
                        {
							LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource->getDesc().
								type == resource_type::Buffer, i, result::ErrorInvalidState)
                            break;
                        }
                        case resource_state::IndexBuffer:
                        {							LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource->getDesc().
								type == resource_type::Buffer, i, result::ErrorInvalidState)
                            break;
                        }
                        case resource_state::ConstantBuffer:
                        {
							LLRI_DETAIL_VALIDATION_REQUIRE_ITER(barriers[i].trans.resource->getDesc().
								type == resource_type::Buffer, i, result::ErrorInvalidState)
                            break;
                        }
                    }
                    
                    break;
                }
            }
        }
#endif

        LLRI_DETAIL_CALL_IMPL(impl_resourceBarrier(numBarriers, barriers), m_validationCallbackMessenger)
    }
    
    inline result CommandList::resourceBarrier(const resource_barrier& barrier)
    {
        return resourceBarrier(1, &barrier);
    }
}
