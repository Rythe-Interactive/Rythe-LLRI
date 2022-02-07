/**
 * @file command_list.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>
#include <graphics/vulkan/volk.h>

namespace llri
{
    result CommandList::impl_begin([[maybe_unused]] const command_list_begin_desc& desc)
    {
        // TODO: Handle nodemask
        // TODO: Handle inheritance/indirect
        VkCommandBufferBeginInfo info { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, {}, nullptr };
        
        const auto r = static_cast<VolkDeviceTable*>(m_deviceFunctionTable)->
            vkBeginCommandBuffer(static_cast<VkCommandBuffer>(m_ptr), &info);
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        m_state = command_list_state::Recording;
        return result::Success;
    }

    result CommandList::impl_end()
    {
        const auto r = static_cast<VolkDeviceTable*>(m_deviceFunctionTable)->
            vkEndCommandBuffer(static_cast<VkCommandBuffer>(m_ptr));
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        m_state = command_list_state::Ready;
        return result::Success;
    }

    result CommandList::impl_resourceBarrier(uint32_t numBarriers, const resource_barrier* barriers)
    {
        uint32_t numMemBarriers = 0, numBufBarriers = 0, numImgBarriers = 0;
        
        std::vector<VkMemoryBarrier> memoryBarriers(numBarriers);
        std::vector<VkBufferMemoryBarrier> bufferBarriers(numBarriers);
        std::vector<VkImageMemoryBarrier> imageBarriers(numBarriers);
        
        for (size_t i = 0; i < numBarriers; i++)
        {
            auto& barrier = barriers[i];
            
            Resource* resource = nullptr;
            switch(barrier.type)
            {
                case resource_barrier_type::Transition:
                    resource = barrier.trans.resource;
                    break;
                case resource_barrier_type::ReadWrite:
                    resource = barrier.rw.resource;
                    break;
            }
            
            auto resourceDesc = resource->getDesc();
            
            if (resourceDesc.type == resource_type::Buffer)
            {
                VkBufferMemoryBarrier bufferBarrier {};
                bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                bufferBarrier.pNext = nullptr;
                
                switch(barrier.type)
                {
                    case resource_barrier_type::Transition:
                    {
                        bufferBarrier.srcAccessMask = internal::mapStateToAccess(barrier.trans.oldState);
                        bufferBarrier.dstAccessMask = internal::mapStateToAccess(barrier.trans.newState);
                        break;
                    }
                    case resource_barrier_type::ReadWrite:
                    {
                        bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
                        bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
                        break;
                    }
                }
                
                bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                
                bufferBarrier.buffer = static_cast<VkBuffer>(resource->m_resource);
                bufferBarrier.offset = 0;
                bufferBarrier.size = resourceDesc.width;
                
                bufferBarriers[numBufBarriers++] = bufferBarrier;
            }
            else
            {
                VkImageMemoryBarrier imgBarrier {};
                imgBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                imgBarrier.pNext = nullptr;
                imgBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imgBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imgBarrier.image = static_cast<VkImage>(resource->m_resource);
                
                switch(barrier.type)
                {
                    case resource_barrier_type::Transition:
                    {
                        imgBarrier.oldLayout = internal::mapResourceState(barrier.trans.oldState);
                        imgBarrier.newLayout = internal::mapResourceState(barrier.trans.newState);
                        
                        imgBarrier.srcAccessMask = internal::mapStateToAccess(barrier.trans.oldState);
                        imgBarrier.dstAccessMask = internal::mapStateToAccess(barrier.trans.newState);
                        break;
                    }
                    case resource_barrier_type::ReadWrite:
                    {
                        imgBarrier.oldLayout = internal::mapResourceState(resource_state::ShaderReadWrite);
                        imgBarrier.newLayout = internal::mapResourceState(resource_state::ShaderReadWrite);
                        
                        imgBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
                        imgBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
                        break;
                    }
                }
                
                // use the texture format to detect the aspect flags
                VkImageAspectFlags aspectFlags = {};
                if (has_color_component(resourceDesc.textureFormat))
                    aspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
                if (has_depth_component(resourceDesc.textureFormat))
                    aspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
                if (has_stencil_component(resourceDesc.textureFormat))
                    aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
                
                // use all subresources for readwrite barriers and if specified in transition.
                bool allSubresources = barrier.type == resource_barrier_type::ReadWrite ||
                    barrier.trans.subresourceRange == texture_subresource_range::all();
                if (allSubresources)
                {
                    // don't accidentally use depthOrArrayLayers for subresources of 3D textures
                    if (resourceDesc.type == resource_type::Texture3D)
                    {
                        imgBarrier.subresourceRange = VkImageSubresourceRange {
                            aspectFlags,
                            0,
                            resourceDesc.mipLevels,
                            0,
                            1
                        };
                    }
                    else
                    {
                        imgBarrier.subresourceRange = VkImageSubresourceRange {
                            aspectFlags,
                            0,
                            resourceDesc.mipLevels,
                            0,
                            resourceDesc.depthOrArrayLayers
                        };
                    }
                }
                else
                {
                    imgBarrier.subresourceRange = VkImageSubresourceRange {
                        aspectFlags,
                        barrier.trans.subresourceRange.baseMipLevel,
                        barrier.trans.subresourceRange.numMipLevels,
                        barrier.trans.subresourceRange.baseArrayLayer,
                        barrier.trans.subresourceRange.numArrayLayers
                    };
                }
                
                imageBarriers[numImgBarriers++] = imgBarrier;
            }
        }
        
        static_cast<VolkDeviceTable*>(m_deviceFunctionTable)->
            vkCmdPipelineBarrier(static_cast<VkCommandBuffer>(m_ptr),
                                 //TODO: Expose pipeline stage for better optimization control.
                                 VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                 VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                 //TODO: Expose dependency for better optimization control.
                                 VK_DEPENDENCY_BY_REGION_BIT,
                                 numMemBarriers,
                                 memoryBarriers.data(),
                                 numBufBarriers,
                                 bufferBarriers.data(),
                                 numImgBarriers,
                                 imageBarriers.data());
        
        return result::Success;
    }

}
