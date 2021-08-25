/**
 * @file device.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
{
    inline result Device::queryQueue(queue_type type, uint8_t index, Queue** queue)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (queue == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Device::queryQueue() returned ErrorInvalidUsage because the passed queue parameter must not be nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

        *queue = nullptr;

#ifndef LLRI_DISABLE_VALIDATION
        if (type > queue_type::MaxEnum)
        {
            const std::string msg = "Device::queryQueue() returned ErrorInvalidUsage because the passed type parameter " + std::to_string((uint8_t)type) + " is not a valid queue_type value.";
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, msg.c_str());
            return result::ErrorInvalidUsage;
        }
#endif

        std::vector<Queue*>* queues = nullptr;
        switch(type)
        {
            case queue_type::Graphics:
            {
                queues = &m_graphicsQueues;
                break;
            }
            case queue_type::Compute:
            {
                queues = &m_computeQueues;
                break;
            }
            case queue_type::Transfer:
            {
                queues = &m_transferQueues;
                break;
            }
        }

#ifndef LLRI_DISABLE_VALIDATION
        if (index >= static_cast<uint8_t>(queues->size()))
        {
            const std::string msg =  "Device::queryQueue() returned ErrorInvalidUsage because the passed index parameter " + std::to_string(index) + " is not smaller than the number of created queues (" + std::to_string(queues->size()) + ") of type " + to_string(type) + ".";
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, msg.c_str());
            return result::ErrorInvalidUsage;
        }
#endif

        *queue = queues->at(index);
        return result::Success;
    }

    inline result Device::createCommandGroup(const command_group_desc& desc, CommandGroup** cmdGroup) const
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (cmdGroup == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Device::createCommandGroup() returned ErrorInvalidUsage because the passed cmdGroup parameter must not be nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

        *cmdGroup = nullptr;

#ifndef LLRI_DISABLE_VALIDATION
        if (desc.type > queue_type::MaxEnum)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Device::createCommandGroup() returned ErrorInvalidUsage because desc.type was not a valid queue_type enum value.");
            return result::ErrorInvalidUsage;
        }

        if (desc.count == 0)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Device::createCommandGroup() returned ErrorInvalidUsage because desc.count was 0");
            return result::ErrorInvalidUsage;
        }

        uint8_t availableQueueCount;
        m_adapter->queryQueueCount(desc.type, &availableQueueCount);
        if (availableQueueCount == 0)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Device::createCommandGroup() returned ErrorInvalidUsage because the the Device's adapter has no queues available for the passed command_group_desc::type.");
            return result::ErrorInvalidUsage;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_createCommandGroup(desc, cmdGroup);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_createCommandGroup(desc, cmdGroup);
#endif
    }

    inline void Device::destroyCommandGroup(CommandGroup* cmdGroup) const
    {
        if (cmdGroup->m_ptr)
        {
            std::vector<CommandList*> cmdLists;
            cmdLists.reserve(cmdGroup->m_cmdLists.size());
            for (auto element : cmdGroup->m_cmdLists)
                cmdLists.push_back(element);
            cmdGroup->free(static_cast<uint8_t>(cmdLists.size()), cmdLists.data());
        }

        impl_destroyCommandGroup(cmdGroup);

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
#endif
    }
}
