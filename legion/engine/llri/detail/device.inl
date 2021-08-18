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
}
