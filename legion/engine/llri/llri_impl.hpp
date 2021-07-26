#pragma once

namespace legion::graphics::llri
{
    constexpr inline const char* to_string(const result& result)
    {
        switch (result)
        {
        case result::Success:
            return "Success";
        case result::Timeout:
            return "Timeout";
        case result::ErrorUnknown:
            return "ErrorUnknown";
        case result::ErrorInvalidUsage:
            return "ErrorInvalidUsage";
        case result::ErrorFeatureNotSupported:
            return "ErrorFeatureNotSupported";
        case result::ErrorExtensionNotSupported:
            return "ErrorExtensionNotSupported";
        case result::ErrorDeviceHung:
            return "ErrorDeviceHung";
        case result::ErrorDeviceLost:
            return "ErrorDeviceLost";
        case result::ErrorDeviceRemoved:
            return "ErrorDeviceRemoved";
        case result::ErrorDriverFailure:
            return "ErrorDriverFailure";
        case result::NotReady:
            return "NotReady";
        case result::ErrorOutOfHostMemory:
            return "ErrorOutOfHostMemory";
        case result::ErrorOutOfDeviceMemory:
            return "ErrorOutOfDeviceMemory";
        case result::ErrorInitializationFailed:
            return "ErrorInitializationFailed";
        case result::ErrorIncompatibleDriver:
            return "ErrorIncompatibleDriver";
        }

        return "Invalid result value";
    }

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
        case validation_callback_source::InternalAPI:
            return "InternalAPI";
        }

        return "Invalid validation_callback_source value";
    }

    constexpr inline const char* to_string(const adapter_type& type)
    {
        switch (type)
        {
        case adapter_type::Other:
            return "Other";
        case adapter_type::Integrated:
            return "Integrated";
        case adapter_type::Discrete:
            return "Discrete";
        case adapter_type::Virtual:
            return "Virtual";
        }

        return "Invalid adapter_type value";
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
        if (desc.numExtensions > 0 && desc.extensions == nullptr)
        {
            if (desc.callbackDesc.callback)
                desc.callbackDesc(validation_callback_severity::Error, validation_callback_source::Validation, "createInstance() returned ErrorInvalidUsage because desc.numExtensions was more than 0 but desc.extensions was nullptr.");
            return result::ErrorInvalidUsage;
        }
#endif

#ifndef LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING
        const auto r =  detail::impl_createInstance(desc, instance, true);
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

#ifndef LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING
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

#ifndef LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING
        const auto r = impl_createDevice(desc, device);
        detail::impl_pollAPIMessages((*device)->m_validationCallback, (*device)->m_validationCallbackMessenger);
        return r;
#else
        return impl_createDevice(desc, device);
#endif
    }

    inline void Instance::destroyDevice(Device* device) const
    {
        impl_destroyDevice(device);

#ifndef LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING
        //Can't use device messenger here because the device is destroyed
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
#endif
    }

    inline result Adapter::queryInfo(adapter_info* info) const
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (info == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryInfo() returned ErrorInvalidUsage because the passed info parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryInfo() returned ErrorDeviceLost because the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }
#endif

#ifndef LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING
        const auto r = impl_queryInfo(info);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_queryInfo(info);
#endif
    }

    inline result Adapter::queryFeatures(adapter_features* features) const
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (features == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryFeatures() returned ErrorInvalidUsage because the passed features parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryFeatures() returned ErrorDeviceLost because the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }
#endif

#ifndef LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING
        const auto r = impl_queryFeatures(features);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_queryFeatures(features);
#endif
    }

    inline result Adapter::queryExtensionSupport(const adapter_extension_type& type, bool* supported) const
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (supported == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryExtensionSupport() returned ErrorInvalidUsage because the passed supported parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryExtensionSupport() returned ErrorDeviceLost because the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }
#endif

#ifndef LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING
        const auto r = impl_queryExtensionSupport(type, supported);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_queryExtensionSupport(type, supported);
#endif
    }
}
