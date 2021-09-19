/**
 * @file queue.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

namespace LLRI_NAMESPACE
{
    result Queue::impl_submit(const submit_desc& desc)
    {
        std::vector<VkCommandBuffer> buffers(desc.numCommandLists);
        for (size_t i = 0; i < desc.numCommandLists; i++)
            buffers[i] = static_cast<VkCommandBuffer>(desc.commandLists[i]->m_ptr);

        std::vector<VkSemaphore> waitSemaphores(desc.numWaitSemaphores);
        std::vector<VkPipelineStageFlags> waitSemaphoreStages(desc.numWaitSemaphores, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
        for (size_t i = 0; i < desc.numWaitSemaphores; i++)
            waitSemaphores[i] = static_cast<VkSemaphore>(desc.waitSemaphores[i]->m_ptr);

        std::vector<VkSemaphore> signalSemaphores(desc.numSignalSemaphores);
        for (size_t i = 0; i < desc.numSignalSemaphores; i++)
            signalSemaphores[i] = static_cast<VkSemaphore>(desc.signalSemaphores[i]->m_ptr);

        VkSubmitInfo info;
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext = nullptr;
        info.commandBufferCount = desc.numCommandLists;
        info.pCommandBuffers = buffers.data();
        info.waitSemaphoreCount = desc.numWaitSemaphores;
        info.pWaitSemaphores = waitSemaphores.data();
        info.signalSemaphoreCount = desc.numSignalSemaphores;
        info.pSignalSemaphores = signalSemaphores.data();
        info.pWaitDstStageMask = waitSemaphoreStages.data();

        VkFence fence = VK_NULL_HANDLE;
        if (desc.fence != nullptr)
        {
            fence = static_cast<VkFence>(desc.fence->m_ptr);
            desc.fence->m_signaled = true;
        }

        const auto r = static_cast<VolkDeviceTable*>(m_device->m_functionTable)->
            vkQueueSubmit(static_cast<VkQueue>(m_ptrs[0]), 1, &info, fence);

        return internal::mapVkResult(r);
    }
}
