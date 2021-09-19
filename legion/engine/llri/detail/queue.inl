/**
 * @file queue.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
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

    inline result Queue::submit(const submit_desc& desc)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if ((desc.nodeMask & (desc.nodeMask - 1)) != 0)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Queue::submit() returned ErrorInvalidNodeMask because desc.nodeMask " + std::to_string(desc.nodeMask) + "has multiple bits set which is not valid for submitting CommandLists.");
            return result::ErrorInvalidNodeMask;
        }

        if (desc.nodeMask >= (1 << m_device->m_adapter->queryNodeCount()))
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Queue::submit() returned ErrorInvalidNodeMask because desc.nodeMask " + std::to_string(desc.nodeMask) + " has a bit set that is more than or at Adapter::queryNodeCount().");
            return result::ErrorInvalidNodeMask;
        }

        if (desc.numCommandLists == 0)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Queue::submit() returned ErrorInvalidUsage because desc.numCommandLists is 0.");
            return result::ErrorInvalidUsage;
        }

        if (desc.commandLists == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Queue::submit() returned ErrorInvalidUsage because desc.commandLists is nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (desc.numWaitSemaphores > 0)
        {
            if (desc.waitSemaphores == nullptr)
            {
                m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Queue::submit() returned ErrorInvalidUsage because desc.numWaitSemaphores is more than 0, but desc.waitSemaphores is nullptr.");
                return result::ErrorInvalidUsage;
            }

            for (size_t i = 0; i < desc.numWaitSemaphores; i++)
            {
                if (desc.waitSemaphores[i] == nullptr)
                {
                    m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Queue::submit() returned ErrorInvalidUsage because desc.numWaitSemaphores is more than 0, but desc.waitSemaphores[" + std::to_string(i) + "] is nullptr.");
                    return result::ErrorInvalidUsage;
                }
            }
        }

        if (desc.numSignalSemaphores > 0)
        {
            if (desc.signalSemaphores == nullptr)
            {
                m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Queue::submit() returned ErrorInvalidUsage because desc.numSignalSemaphores is more than 0, but desc.signalSemaphores is nullptr.");
                return result::ErrorInvalidUsage;
            }

            for (size_t i = 0; i < desc.numSignalSemaphores; i++)
            {
                if (desc.signalSemaphores[i] == nullptr)
                {
                    m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Queue::submit() returned ErrorInvalidUsage because desc.numSignalSemaphores is more than 0, but desc.signalSemaphores[" + std::to_string(i) + "] is nullptr.");
                    return result::ErrorInvalidUsage;
                }
            }
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_submit(desc);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_submit(desc);
#endif
    }
}
