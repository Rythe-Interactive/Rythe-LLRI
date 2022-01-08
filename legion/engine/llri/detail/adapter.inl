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

    inline adapter_info Adapter::queryInfo() const
    {
        LLRI_DETAIL_CALL_IMPL(impl_queryInfo(), m_validationCallbackMessenger)
    }

    inline adapter_features Adapter::queryFeatures() const
    {
        LLRI_DETAIL_CALL_IMPL(impl_queryFeatures(), m_validationCallbackMessenger)
    }

    inline adapter_limits Adapter::queryLimits() const
    {
        LLRI_DETAIL_CALL_IMPL(impl_queryLimits(), m_validationCallbackMessenger)
    }

    inline bool Adapter::queryExtensionSupport(adapter_extension ext) const
    {
        LLRI_DETAIL_CALL_IMPL(impl_queryExtensionSupport(ext), m_validationCallbackMessenger)
    }

    inline uint8_t Adapter::queryQueueCount(queue_type type) const
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(type <= queue_type::MaxEnum, 0)
        LLRI_DETAIL_CALL_IMPL(impl_queryQueueCount(type), m_validationCallbackMessenger)
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
