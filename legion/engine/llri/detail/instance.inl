/**
 * Copyright 2021-2021 Leon Brands. All rights served.
 * License: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
{
    constexpr inline const char* to_string(const validation_callback_severity& severity)
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

    constexpr inline const char* to_string(const validation_callback_source& source)
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

    constexpr inline const char* to_string(const instance_extension_type& result)
    {
        switch (result)
        {
        case instance_extension_type::APIValidation:
            return "APIValidation";
        case instance_extension_type::GPUValidation:
            return "GPUValidation";
        }

        return "Invalid instance_extension_type value";
    }

    [[nodiscard]] inline bool queryInstanceExtensionSupport(const instance_extension_type& type)
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

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_enumerateAdapters(adapters);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_enumerateAdapters(adapters);
#endif
    }

    inline result Instance::createDevice(const device_desc& desc, Device** device) const
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (device == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Instance::createDevice() returned ErrorInvalidUsage because the passed device parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

        //default device output to nullptr
        * device = nullptr;

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

    inline void Instance::destroyDevice(Device* device) const
    {
        impl_destroyDevice(device);

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        //Can't use device messenger here because the device is destroyed
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
#endif
    }
}
