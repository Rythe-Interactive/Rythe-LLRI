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
        LLRI_DETAIL_VALIDATION_REQUIRE(info != nullptr, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(m_ptr != nullptr, result::ErrorDeviceLost)

        LLRI_DETAIL_CALL_IMPL(impl_queryInfo(info), m_validationCallbackMessenger)
    }

    inline result Adapter::queryFeatures(adapter_features* features) const
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(features != nullptr, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(m_ptr != nullptr, result::ErrorDeviceLost)

        LLRI_DETAIL_CALL_IMPL(impl_queryFeatures(features), m_validationCallbackMessenger)
    }

    inline adapter_limits Adapter::queryLimits() const
    {
        LLRI_DETAIL_CALL_IMPL(impl_queryLimits(), m_validationCallbackMessenger)
    }

    inline bool Adapter::queryExtensionSupport(adapter_extension ext) const
    {
        LLRI_DETAIL_CALL_IMPL(impl_queryExtensionSupport(ext), m_validationCallbackMessenger)
    }

    inline result Adapter::queryQueueCount(queue_type type, uint8_t* count) const
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(count != nullptr, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(type <= queue_type::MaxEnum, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(m_ptr != nullptr, result::ErrorDeviceLost)

        *count = 0;

        LLRI_DETAIL_CALL_IMPL(impl_queryQueueCount(type, count), m_validationCallbackMessenger)
    }

    inline const std::unordered_map<format, format_properties>& Adapter::queryFormatProperties() const
    {
        if (m_cachedFormatProperties.empty())
        {
            m_cachedFormatProperties = impl_queryFormatProperties();
            LLRI_DETAIL_POLL_API_MESSAGES(m_validationCallbackMessenger)
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
