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

        VkCommandBufferLevel mapCommandListUsage(command_list_usage usage)
        {
            switch(usage)
            {
                case command_list_usage::Direct:
                    return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                case command_list_usage::Indirect:
                    return VK_COMMAND_BUFFER_LEVEL_SECONDARY;
            }

            throw;
        }
    }

    result Device::impl_createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup) const
    {
        auto* output = new CommandGroup();
        output->m_deviceHandle = m_ptr;
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

    void Device::impl_destroyCommandGroup(CommandGroup* cmdGroup) const
    {
        if (!cmdGroup)
            return;

        if (cmdGroup->m_ptr)
            static_cast<VolkDeviceTable*>(m_functionTable)->vkDestroyCommandPool(static_cast<VkDevice>(m_ptr), static_cast<VkCommandPool>(cmdGroup->m_ptr), nullptr);

        delete cmdGroup;
    }

    result Device::impl_allocateCommandList(const command_list_alloc_desc& desc, CommandList** cmdList) const
    {
        VkCommandBufferAllocateInfo allocInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr };
        allocInfo.commandPool = static_cast<VkCommandPool>(desc.group->m_ptr);
        allocInfo.commandBufferCount = 1;
        allocInfo.level = internal::mapCommandListUsage(desc.usage);

        VkCommandBuffer cmd;
        auto r = static_cast<VolkDeviceTable*>(m_functionTable)->vkAllocateCommandBuffers(static_cast<VkDevice>(m_ptr), &allocInfo, &cmd);
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        auto* output = new CommandList();
        output->m_ptr = cmd;
        output->m_state = command_list_state::Empty;
        output->m_validationCallback = m_validationCallback;
        output->m_validationCallbackMessenger = m_validationCallbackMessenger;
        desc.group->m_cmdLists.push_back(output);

        *cmdList = output;
        return result::Success;
    }

    result Device::impl_allocateCommandLists(const command_list_alloc_desc& desc, uint8_t count, std::vector<CommandList*>* cmdLists) const
    {
        VkCommandBufferAllocateInfo allocInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr };
        allocInfo.commandPool = static_cast<VkCommandPool>(desc.group->m_ptr);
        allocInfo.commandBufferCount = count;
        allocInfo.level = internal::mapCommandListUsage(desc.usage);

        std::vector<VkCommandBuffer> cmdBuffers(count);
        auto r = static_cast<VolkDeviceTable*>(m_functionTable)->vkAllocateCommandBuffers(static_cast<VkDevice>(m_ptr), &allocInfo, cmdBuffers.data());
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        for (uint8_t i = 0; i < count; i++)
        {
            auto* cmdList = new CommandList();
            cmdList->m_ptr = cmdBuffers[i];
            cmdList->m_state = command_list_state::Empty;
            cmdList->m_validationCallback = m_validationCallback;
            cmdList->m_validationCallbackMessenger = m_validationCallbackMessenger;

            cmdLists->push_back(cmdList);
            desc.group->m_cmdLists.push_back(cmdList);
        }

        return result::Success;
    }

    result Device::impl_freeCommandList(CommandGroup* group, CommandList* cmdList) const
    {
        //Free internal pointer
        auto buffer = static_cast<VkCommandBuffer>(cmdList->m_ptr);
        static_cast<VolkDeviceTable*>(m_functionTable)
            ->vkFreeCommandBuffers(static_cast<VkDevice>(m_ptr), static_cast<VkCommandPool>(group->m_ptr), 1, &buffer);

        //Remove from commandlist list
        group->m_cmdLists.remove(cmdList);

        //Delete wrapper
        delete cmdList;
        return result::Success;
    }

    result Device::impl_freeCommandLists(CommandGroup* group, uint8_t numCommandLists, CommandList** cmdLists) const
    {
        //Gather vk handles
        std::vector<VkCommandBuffer> buffers(numCommandLists);
        for (uint8_t i = 0; i < numCommandLists; i++)
            buffers[i] = static_cast<VkCommandBuffer>(cmdLists[i]->m_ptr);

        //Free internal pointers
        static_cast<VolkDeviceTable*>(m_functionTable)
            ->vkFreeCommandBuffers(static_cast<VkDevice>(m_ptr), static_cast<VkCommandPool>(group->m_ptr), buffers.size(), buffers.data());

        for (uint8_t i = 0; i < numCommandLists; i++)
        {
            //Remove from commandlist list
            group->m_cmdLists.remove(cmdLists[i]);

            //Delete wrapper
            delete cmdLists[i];
        }

        return result::Success;
    }
}
