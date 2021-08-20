/**
 * @file command_group.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

namespace LLRI_NAMESPACE
{
    result CommandGroup::impl_reset()
    {
        return internal::mapVkResult(static_cast<VolkDeviceTable*>(m_deviceFunctionTable)->
            vkResetCommandPool(static_cast<VkDevice>(m_deviceHandle), static_cast<VkCommandPool>(m_ptr), VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
    }
}
