/**
 * @file device.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

namespace LLRI_NAMESPACE
{
    namespace internal
    {
        std::map<queue_type, uint32_t> findQueueFamilies(VkPhysicalDevice physicalDevice);
    }

    result Device::impl_createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup)
    {
        auto* output = new CommandGroup();
        output->m_device = this;
        output->m_deviceFunctionTable = m_functionTable;
        output->m_validationCallback = m_validationCallback;
        output->m_validationCallbackMessenger = m_validationCallbackMessenger;
        output->m_maxCount = desc.count;
        output->m_type = desc.type;

        auto families = internal::findQueueFamilies(static_cast<VkPhysicalDevice>(m_adapter->m_ptr));

        VkCommandPoolCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.queueFamilyIndex = families[desc.type];
        info.flags = {};

        VkCommandPool pool;
        const auto r = static_cast<VolkDeviceTable*>(m_functionTable)->vkCreateCommandPool(static_cast<VkDevice>(m_ptr), &info, nullptr, &pool);
        if (r != VK_SUCCESS)
        {
            destroyCommandGroup(output);
            return internal::mapVkResult(r);
        }

        output->m_ptr = pool;
        *cmdGroup = output;
        return result::Success;
    }

    void Device::impl_destroyCommandGroup(CommandGroup* cmdGroup)
    {
        if (!cmdGroup)
            return;

        if (cmdGroup->m_ptr)
            static_cast<VolkDeviceTable*>(m_functionTable)->vkDestroyCommandPool(static_cast<VkDevice>(m_ptr), static_cast<VkCommandPool>(cmdGroup->m_ptr), nullptr);

        delete cmdGroup;
    }
}
