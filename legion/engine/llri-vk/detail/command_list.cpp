/**
 * @file command_list.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>
#include <graphics/vulkan/volk.h>

namespace LLRI_NAMESPACE
{
    result CommandList::impl_begin(const command_list_begin_desc& desc)
    {
        //TODO: Handle nodemask
        //TODO: Handle inheritance/indirect
        VkCommandBufferBeginInfo info { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, {}, nullptr };
        
        const auto r = static_cast<VolkDeviceTable*>(m_deviceFunctionTable)->vkBeginCommandBuffer(static_cast<VkCommandBuffer>(m_ptr), &info);
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        m_state = command_list_state::Recording;
        return result::Success;
    }

    result CommandList::impl_end()
    {
        const auto r = static_cast<VolkDeviceTable*>(m_deviceFunctionTable)->vkEndCommandBuffer(static_cast<VkCommandBuffer>(m_ptr));
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        m_state = command_list_state::Ready;
        return result::Success;
    }
}
