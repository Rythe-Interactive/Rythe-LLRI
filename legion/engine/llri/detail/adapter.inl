/**
 * @file adapter.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    inline std::string to_string(adapter_type type)
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
            detail::apiError("Adapter::queryInfo()", result::ErrorInvalidUsage, "the passed info parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            detail::apiError("Adapter::queryInfo()", result::ErrorDeviceLost, "the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_queryInfo(info);
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
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
            detail::apiError("Adapter::queryFeatures()", result::ErrorInvalidUsage, "the passed features parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            detail::apiError("Adapter::queryFeatures()", result::ErrorDeviceLost, "the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }
#endif

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_queryFeatures(features);
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
        return r;
#else
        return impl_queryFeatures(features);
#endif
    }

    inline adapter_limits Adapter::queryLimits() const
    {
#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto output = impl_queryLimits();
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
        return output;
#else
        return impl_queryLimits();
#endif
    }

    inline result Adapter::queryExtensionSupport(adapter_extension_type type, bool* supported) const
    {
#ifndef LLRI_DISABLE_VALIDATION
        if (supported == nullptr)
        {
            detail::apiError("Adapter::queryExtensionSupport()", result::ErrorInvalidUsage, "the passed supported parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            detail::apiError("Adapter::queryExtensionSupport()", result::ErrorDeviceLost, "the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }
#endif

        *supported = false;

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_queryExtensionSupport(type, supported);
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
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
            detail::apiError("Adapter::queryQueueCount()", result::ErrorInvalidUsage, "the passed count parameter was nullptr.");
            return result::ErrorInvalidUsage;
        }

        if (type > queue_type::MaxEnum)
        {
            detail::apiError("Adapter::queryQueueCount()", result::ErrorInvalidUsage, "the passed type parameter was not a valid queue_type value");
            return result::ErrorInvalidUsage;
        }

        if (m_ptr == nullptr)
        {
            detail::apiError("Adapter::queryQueueCount()", result::ErrorDeviceLost, "the passed adapter has a nullptr internal handle which usually indicates a lost device.");
            return result::ErrorDeviceLost;
        }
#endif

        *count = 0;

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        const auto r = impl_queryQueueCount(type, count);
        detail::impl_pollAPIMessages(m_validationCallbackMessenger);
        return r;
#else
        return impl_queryQueueCount(type, count);
#endif
    }

    inline const std::unordered_map<format, format_properties>& Adapter::queryFormatProperties() const
    {
        if (m_cachedFormatProperties.empty())
        {
            m_cachedFormatProperties = impl_queryFormatProperties();

#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
            detail::impl_pollAPIMessages(m_validationCallbackMessenger);
#endif
        }

        return m_cachedFormatProperties;
    }

    inline format_properties Adapter::queryFormatProperties(format f) const
    {
        return queryFormatProperties().at(f);
    }

    inline uint8_t Adapter::queryNodeCount() const
    {
        return m_nodeCount;
    }
}
