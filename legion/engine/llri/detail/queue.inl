/**
 * @file queue.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    inline std::string to_string(queue_priority priority)
    {
        switch(priority)
        {
            case queue_priority::Normal:
                return "Normal";
            case queue_priority::High:
                return "High";
        }
        return "Invalid queue_priority value";
    }

    inline std::string to_string(queue_type type)
    {
        switch(type)
        {
            case queue_type::Graphics:
                return "Graphics";
            case queue_type::Compute:
                return "Compute";
            case queue_type::Transfer:
                return "Transfer";
        }

        return "Invalid queue_type value";
    }

    inline queue_desc Queue::getDesc() const
    {
        return m_desc;
    }

    inline Queue::native_queue* Queue::getNative(size_t index) const
    {
        if (index >= m_ptrs.size())
            return nullptr;
        
        return m_ptrs[index];
    }

    inline result Queue::submit(const submit_desc& desc)
    {
#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        LLRI_DETAIL_VALIDATION_REQUIRE(detail::hasSingleBit(desc.nodeMask), result::ErrorInvalidNodeMask)
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.nodeMask < (1 << m_device->m_adapter->queryNodeCount()), result::ErrorInvalidNodeMask)

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.numCommandLists != 0, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.commandLists != nullptr, result::ErrorInvalidUsage)

        for (size_t i = 0; i < desc.numCommandLists; i++)
        {
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(desc.commandLists[i] != nullptr, i, result::ErrorInvalidUsage)
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(desc.commandLists[i]->getState() == llri::command_list_state::Ready, i, result::ErrorInvalidState)

            const uint32_t descNodeMask = desc.nodeMask == 0 ? 1 : desc.nodeMask;
            const uint32_t cmdListNodeMask = desc.commandLists[i]->m_desc.nodeMask == 0 ? 1 : desc.commandLists[i]->m_desc.nodeMask;

            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(descNodeMask == cmdListNodeMask, i, result::ErrorIncompatibleNodeMask)
        }

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.numWaitSemaphores > 0, desc.waitSemaphores != nullptr, result::ErrorInvalidUsage)
        for (size_t i = 0; i < desc.numWaitSemaphores; i++)
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(desc.waitSemaphores[i] != nullptr, i, result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.numSignalSemaphores > 0, desc.signalSemaphores != nullptr, result::ErrorInvalidUsage)
        for (size_t i = 0; i < desc.numSignalSemaphores; i++)
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(desc.signalSemaphores[i] != nullptr, i, result::ErrorInvalidUsage)

        LLRI_DETAIL_VALIDATION_REQUIRE_IF(desc.fence != nullptr, desc.fence->m_signaled == false, result::ErrorAlreadySignaled)
#endif

        LLRI_DETAIL_CALL_IMPL(impl_submit(desc), m_validationCallbackMessenger)
    }

    inline result Queue::waitIdle()
    {
        LLRI_DETAIL_CALL_IMPL(impl_waitIdle(), m_validationCallbackMessenger)
    }
}
