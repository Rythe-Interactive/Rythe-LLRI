/**
 * @file device.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

namespace LLRI_NAMESPACE
{
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
        const auto r = static_cast<VolkDeviceTable*>(m_functionTable)->
            vkCreateCommandPool(static_cast<VkDevice>(m_ptr), &info, nullptr, &pool);
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
        {
            static_cast<VolkDeviceTable*>(m_functionTable)->
                vkDestroyCommandPool(static_cast<VkDevice>(m_ptr), static_cast<VkCommandPool>(cmdGroup->m_ptr), nullptr);
        }

        delete cmdGroup;
    }

    result Device::impl_createFence(fence_flags flags, Fence** fence)
    {
        const bool signaled = (flags & fence_flag_bits::Signaled) == fence_flag_bits::Signaled;

        VkFenceCreateFlags vkFlags = 0;
        if (signaled)
            vkFlags |= VK_FENCE_CREATE_SIGNALED_BIT;

        VkFenceCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = vkFlags;

        VkFence vkFence;
        const auto r = static_cast<VolkDeviceTable*>(m_functionTable)->
            vkCreateFence(static_cast<VkDevice>(m_ptr), &info, nullptr, &vkFence);
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        auto* output = new Fence();
        output->m_ptr = vkFence;
        output->m_signaled = signaled;

        *fence = output;
        return result::Success;
    }

    void Device::impl_destroyFence(Fence* fence)
    {
        if (fence->m_ptr)
        {
             static_cast<VolkDeviceTable*>(m_functionTable)->
                vkDestroyFence(static_cast<VkDevice>(m_ptr), static_cast<VkFence>(fence->m_ptr), nullptr);
        }

        delete fence;
    }

    result Device::impl_waitFences(uint32_t numFences, Fence** fences, uint64_t timeout)
    {
        uint64_t vkTimeout = timeout;
        if (timeout != LLRI_TIMEOUT_INFINITE)
            vkTimeout *= 1000000u; // milliseconds to nanoseconds

        std::vector<VkFence> vkFences(numFences);
        for (size_t i = 0; i < numFences; i++)
            vkFences[i] = static_cast<VkFence>(fences[i]->m_ptr);

        const VkResult r = static_cast<VolkDeviceTable*>(m_functionTable)->
            vkWaitForFences(static_cast<VkDevice>(m_ptr), numFences, vkFences.data(), true, vkTimeout);

        if (r == VK_SUCCESS)
        {
            static_cast<VolkDeviceTable*>(m_functionTable)->
                vkResetFences(static_cast<VkDevice>(m_ptr), numFences, vkFences.data());
        }

        return internal::mapVkResult(r);
    }
}
