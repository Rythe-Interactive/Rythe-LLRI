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
        const auto r = static_cast<VolkDeviceTable*>(m_deviceFunctionTable)->
            vkResetCommandPool(static_cast<VkDevice>(m_deviceHandle), static_cast<VkCommandPool>(m_ptr), VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        for (auto* cmdList : m_cmdLists)
            cmdList->m_state = command_list_state::Empty;

        return result::Success;
    }

    result CommandGroup::impl_allocate(const command_list_alloc_desc& desc, CommandList** cmdList)
    {
        VkCommandBufferAllocateInfo allocInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr };
        allocInfo.commandPool = static_cast<VkCommandPool>(m_ptr);
        allocInfo.commandBufferCount = 1;
        allocInfo.level = internal::mapCommandListUsage(desc.usage);

        VkCommandBuffer cmd;
        auto r = static_cast<VolkDeviceTable*>(m_deviceFunctionTable)->vkAllocateCommandBuffers(static_cast<VkDevice>(m_deviceHandle), &allocInfo, &cmd);
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        auto* output = new CommandList();
        output->m_ptr = cmd;
        output->m_groupHandle = m_ptr;

        output->m_deviceHandle = m_deviceHandle;
        output->m_deviceFunctionTable = m_deviceFunctionTable;

        output->m_usage = desc.usage;
        output->m_state = command_list_state::Empty;

        output->m_validationCallback = m_validationCallback;
        output->m_validationCallbackMessenger = m_validationCallbackMessenger;

        m_cmdLists.push_back(output);

        *cmdList = output;
        return result::Success;
    }

    result CommandGroup::impl_allocate(const command_list_alloc_desc& desc, uint8_t count, std::vector<CommandList*>* cmdLists)
    {
        VkCommandBufferAllocateInfo allocInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr };
        allocInfo.commandPool = static_cast<VkCommandPool>(m_ptr);
        allocInfo.commandBufferCount = count;
        allocInfo.level = internal::mapCommandListUsage(desc.usage);

        std::vector<VkCommandBuffer> cmdBuffers(count);
        auto r = static_cast<VolkDeviceTable*>(m_deviceFunctionTable)->vkAllocateCommandBuffers(static_cast<VkDevice>(m_deviceHandle), &allocInfo, cmdBuffers.data());
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        for (uint8_t i = 0; i < count; i++)
        {
            auto* cmdList = new CommandList();
            cmdList->m_ptr = cmdBuffers[i];
            cmdList->m_groupHandle = m_ptr;

            cmdList->m_deviceHandle = m_deviceHandle;
            cmdList->m_deviceFunctionTable = m_deviceFunctionTable;

            cmdList->m_usage = desc.usage;
            cmdList->m_state = command_list_state::Empty;

            cmdList->m_validationCallback = m_validationCallback;
            cmdList->m_validationCallbackMessenger = m_validationCallbackMessenger;

            m_cmdLists.push_back(cmdList);
            cmdLists->push_back(cmdList);
        }

        return result::Success;
    }

    result CommandGroup::impl_free(CommandList* cmdList)
    {
        //Free internal pointer
        auto buffer = static_cast<VkCommandBuffer>(cmdList->m_ptr);
        static_cast<VolkDeviceTable*>(m_deviceFunctionTable)
            ->vkFreeCommandBuffers(static_cast<VkDevice>(m_deviceHandle), static_cast<VkCommandPool>(m_ptr), 1, &buffer);

        //Remove from commandlist list
        m_cmdLists.remove(cmdList);

        //Delete wrapper
        delete cmdList;
        return result::Success;
    }

    result CommandGroup::impl_free(uint8_t numCommandLists, CommandList** cmdLists)
    {
        //Gather vk handles
        std::vector<VkCommandBuffer> buffers(numCommandLists);
        for (uint8_t i = 0; i < numCommandLists; i++)
            buffers[i] = static_cast<VkCommandBuffer>(cmdLists[i]->m_ptr);

        //Free internal pointers
        static_cast<VolkDeviceTable*>(m_deviceFunctionTable)
            ->vkFreeCommandBuffers(static_cast<VkDevice>(m_deviceHandle), static_cast<VkCommandPool>(m_ptr), static_cast<uint32_t>(buffers.size()), buffers.data());

        for (uint8_t i = 0; i < numCommandLists; i++)
        {
            //Remove from commandlist list
            m_cmdLists.remove(cmdLists[i]);

            //Delete wrapper
            delete cmdLists[i];
        }

        return result::Success;
    }
}
