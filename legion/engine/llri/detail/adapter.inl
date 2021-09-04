/**
 * @file adapter.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
{
    constexpr inline const char* to_string(adapter_type type)
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

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
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

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_queryFeatures(features);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_queryFeatures(features);
#endif
    }

    inline result Adapter::queryExtensionSupport(adapter_extension_type type, bool* supported) const
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

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_queryExtensionSupport(type, supported);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_queryExtensionSupport(type, supported);
#endif
    }

    inline result Adapter::queryQueueCount(queue_type type, uint8_t* count) const
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (count == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryQueueCount() returned ErrorInvalidUsage because the passed count parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (type > queue_type::MaxEnum)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryQueueCount() returned ErrorInvalidUsage because the passed type parameter was not a valid queue_type value");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryQueueCount() returned ErrorDeviceLost because the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_queryQueueCount(type, count);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_queryQueueCount(type, count);
#endif
    }

    inline result Adapter::queryNodeCountEXT(uint8_t* count) const
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (count == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryNodeCountEXT() returned ErrorInvalidUsage because the passed count parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryNodeCountEXT() returned ErrorDeviceLost because the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }

        if (!m_instanceValidationData.adapterNodesEnabled)
        {
            m_validationCallback(validation_callback_severity::Error, validation_callback_source::Validation, "Adapter::queryNodeCountEXT() returned ErrorExtensionNotEnabled because the adapter_nodes_ext extension wasn't enabled in Instance");
            return result::ErrorExtensionNotEnabled;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_queryNodeCountEXT(count);
        detail::impl_pollAPIMessages(m_validationCallback, m_validationCallbackMessenger);
        return r;
#else
        return impl_queryNodeCountEXT(count);
#endif
    }
}
