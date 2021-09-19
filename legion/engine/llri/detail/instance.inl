/**
 * @file instance.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
{
    inline std::string to_string(validation_callback_severity severity)
    {
        switch (severity)
        {
        case validation_callback_severity::Verbose:
            return "Verbose";
        case validation_callback_severity::Info:
            return "Info";
        case validation_callback_severity::Warning:
            return "Warning";
        case validation_callback_severity::Error:
            return "Error";
        case validation_callback_severity::Corruption:
            return "Corruption";
        }

        return "Invalid validation_callback_severity value";
    }

    inline std::string to_string(validation_callback_source source)
    {
        switch (source)
        {
        case validation_callback_source::Validation:
            return "Validation";
        case validation_callback_source::Implementation:
            return "Implementation";
        }

        return "Invalid validation_callback_source value";
    }

    inline std::string to_string(instance_extension_type type)
    {
        switch (type)
        {
            case instance_extension_type::DriverValidation:
                return "DriverValidation";
            case instance_extension_type::GPUValidation:
                return "GPUValidation";
        }

        return "Invalid instance_extension_type value";
    }

    [[nodiscard]] inline bool queryInstanceExtensionSupport(instance_extension_type type)
    {
        return detail::queryInstanceExtensionSupport(type);
    }

    inline result createInstance(const instance_desc& desc, Instance** instance)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (instance == nullptr)
        {
            if (desc.callbackDesc.callback)
                desc.callbackDesc(validation_callback_severity::Error, validation_callback_source::Validation, "createInstance() returned ErrorInvalidUsage because the passed instance parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

        //default instance output to nullptr
        * instance = nullptr;

#ifndef LLRI_DISABLE_VALIDATION
        if (desc.numExtensions > 0 && desc.extensions == nullptr)
        {
            if (desc.callbackDesc.callback)
                desc.callbackDesc(validation_callback_severity::Error, validation_callback_source::Validation, "createInstance() returned ErrorInvalidUsage because desc.numExtensions was more than 0 but desc.extensions was nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = detail::impl_createInstance(desc, instance, true);
        if (*instance)
            detail::impl_pollAPIMessages((*instance)->m_validationCallback, (*instance)->m_validationCallbackMessenger);
        return r;
#else
        return detail::impl_createInstance(desc, instance, false);
#endif
    }

    inline void destroyInstance(Instance* instance)
    {
        if (!instance)
            return;
        
        detail::impl_destroyInstance(instance);
        //Can't do any polling after the instance is destroyed
    }

    inline result Instance::enumerateAdapters(std::vector<Adapter*>* adapters)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (adapters == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Instance::enumerateAdapters() returned ErrorInvalidUsage because the passed adapters parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

        adapters->clear();

        //Clear internal pointers, lost adapters will have a nullptr m_ptr
        for (auto& [ptr, adapter] : m_cachedAdapters)
            adapter->m_ptr = nullptr;

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_enumerateAdapters(adapters);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_enumerateAdapters(adapters);
#endif
    }

    inline result Instance::createDevice(const device_desc& desc, Device** device)
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (device == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Instance::createDevice() returned ErrorInvalidUsage because the passed device parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

        //default device output to nullptr
        *device = nullptr;

#ifndef LLRI_DISABLE_VALIDATION
        if (desc.adapter == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Instance::createDevice() returned ErrorInvalidUsage because desc.adapter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (desc.numExtensions > 0 && desc.extensions == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Instance::createDevice() returned ErrorInvalidUsage because desc.numExtensions was more than 0 but desc.extensions was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (desc.adapter->m_ptr == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Instance::createDevice() returned ErrorDeviceLost because the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }

        if (desc.numQueues == 0)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Instance::createDevice() returned ErrorInvalidUsage because desc.numQueues is 0 but it must be at least 1.");
            return result::ErrorInvalidUsage;
        }

        if (desc.queues == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Instance::createDevice() returned ErrorInvalidUsage because desc.queues is nullptr but it must be a valid pointer to an array of queue_desc of size desc.numQueues.");
            return result::ErrorInvalidUsage;
        }

        //Get max queues
        std::map<queue_type, uint8_t> maxQueueCounts {
            { queue_type::Graphics, 0 },
            { queue_type::Compute, 0 },
            { queue_type::Transfer, 0 }
        };
        desc.adapter->queryQueueCount(queue_type::Graphics, &maxQueueCounts[queue_type::Graphics]);
        desc.adapter->queryQueueCount(queue_type::Compute, &maxQueueCounts[queue_type::Compute]);
        desc.adapter->queryQueueCount(queue_type::Transfer, &maxQueueCounts[queue_type::Transfer]);

        //Validate all queue descs and their relation to max queue counts
        std::map<queue_type, uint8_t> queueCounts {
            { queue_type::Graphics, 0 },
            { queue_type::Compute, 0 },
            { queue_type::Transfer, 0 }
        };

        for (size_t i = 0; i < desc.numQueues; i++)
        {
            auto& queue = desc.queues[i];

            //Queue type must be valid
            if (queue.type > queue_type::MaxEnum)
            {
                const std::string msg = "Instance::createDevice() returned ErrorInvalidUsage because queue_desc[" + std::to_string(i) + "]::type " + std::to_string((uint8_t)queue.type) + " is not a valid queue type.";
                m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, msg.c_str());
                return result::ErrorInvalidUsage;
            }

            //Make sure that the requested number of queues of this given type aren't more than the max number of queues of that type.
            queueCounts[queue.type]++;
            if (queueCounts[queue.type] > maxQueueCounts[queue.type])
            {
                const std::string msg = "Instance::createDevice() returned ErrorInvalidUsage because queue_desc " + std::to_string(i) +
                    " is the " + std::to_string(queueCounts[queue.type]) + "th " +
                    to_string(queue.type) + " queue, even though the maximum number of queues of this type is " + std::to_string(maxQueueCounts[queue.type]) + ".";

                m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, msg.c_str());
                return result::ErrorInvalidUsage;
            }

            //Queue priority must be valid
            if (queue.priority > queue_priority::MaxEnum)
            {
                const std::string msg = "Instance::createDevice() returned ErrorInvalidUsage because queue_desc[" + std::to_string(i) + "]::priority " + std::to_string((uint8_t)queue.priority) + " is not a valid priority value";
                m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, msg.c_str());
                return result::ErrorInvalidUsage;
            }
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_createDevice(desc, device);
        if (*device)
            detail::impl_pollAPIMessages((*device)->m_validationCallback, (*device)->m_validationCallbackMessenger);
        return r;
#else
        return impl_createDevice(desc, device);
#endif
    }

    inline void Instance::destroyDevice(Device* device)
    {
        if (!device)
            return;
        
        impl_destroyDevice(device);

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        //Can't use device messenger here because the device is destroyed
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
#endif
    }
}
