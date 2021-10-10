/**
 * @file adapter.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>
#include <graphics/vulkan/volk.h>

namespace LLRI_NAMESPACE
{
    namespace internal
    {
        constexpr adapter_type mapPhysicalDeviceType(VkPhysicalDeviceType type)
        {
            switch (type)
            {
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    return adapter_type::Other;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    return adapter_type::Integrated;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    return adapter_type::Discrete;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    return adapter_type::Virtual;
                default:
                    break;
            }

            return adapter_type::Other;
        }
    }

    result Adapter::impl_queryInfo(adapter_info* info) const
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(static_cast<VkPhysicalDevice>(m_ptr), &properties);

        adapter_info result;
        result.vendorId = properties.vendorID;
        result.adapterId = properties.deviceID;
        result.adapterName = properties.deviceName;
        result.adapterType = internal::mapPhysicalDeviceType(properties.deviceType);
        *info = result;
        return result::Success;
    }

    result Adapter::impl_queryFeatures(adapter_features* features) const
    {
        VkPhysicalDeviceFeatures physicalFeatures;
        vkGetPhysicalDeviceFeatures(static_cast<VkPhysicalDevice>(m_ptr), &physicalFeatures);

        adapter_features result;

        //Set all the information in a structured way here

        *features = result;
        return result::Success;
    }

    result Adapter::impl_queryExtensionSupport(adapter_extension_type type, bool* supported) const
    {
        switch (type)
        {
            default:
                break;
        }

        return result::Success;
    }

    result Adapter::impl_queryQueueCount(queue_type type, uint8_t* count) const
    {
        //Get queue family info
        uint32_t propertyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(static_cast<VkPhysicalDevice>(m_ptr), &propertyCount, nullptr);
        std::vector<VkQueueFamilyProperties> properties(propertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(static_cast<VkPhysicalDevice>(m_ptr), &propertyCount, properties.data());

        for (auto p : properties)
        {
            switch(type)
            {
                case queue_type::Graphics:
                {
                    //Only the graphics queue has the graphics bit set
                    //it usually also has compute & transfer set, because graphics queue tends to be general purpose
                    if ((p.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
                    {
                        *count = static_cast<uint8_t>(p.queueCount);
                        return result::Success;
                    }
                    break;
                }
                case queue_type::Compute:
                {
                    //Dedicated compute family has no graphics bit but does have a compute bit
                    if ((p.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
                        (p.queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT)
                    {
                        *count = static_cast<uint8_t>(p.queueCount);
                        return result::Success;
                    }
                    break;
                }
                case queue_type::Transfer:
                {
                    //Dedicated transfer family has no graphics bit, no compute bit, but does have a transfer bit
                    if ((p.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
                        (p.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0 &&
                        (p.queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT)
                    {
                        *count = static_cast<uint8_t>(p.queueCount);
                        return result::Success;
                    }
                    break;
                }
            }
        }

        return result::Success;
    }

    std::unordered_map<format, format_properties> Adapter::impl_queryFormatProperties() const
    {
        std::unordered_map<format, format_properties> result;

        for (uint8_t f = 0; f <= static_cast<uint8_t>(format::MaxEnum); f++)
        {
            const auto form = static_cast<format>(f);
            const auto vkFormat = internal::mapTextureFormat(form);

            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties(static_cast<VkPhysicalDevice>(m_ptr), vkFormat, &properties);

            const bool supported = properties.bufferFeatures == 0 && properties.optimalTilingFeatures == 0;

            resource_usage_flags usageFlags = resource_usage_flag_bits::None;

            if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT) == VK_FORMAT_FEATURE_TRANSFER_SRC_BIT)
                usageFlags |= resource_usage_flag_bits::TransferSrc;

            if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_DST_BIT) == VK_FORMAT_FEATURE_TRANSFER_DST_BIT)
                usageFlags |= resource_usage_flag_bits::TransferDst;

            if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) == VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)
                usageFlags |= resource_usage_flag_bits::Sampled;

            if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) == VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)
                usageFlags |= resource_usage_flag_bits::ShaderWrite;

            if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)
                usageFlags |= resource_usage_flag_bits::ColorAttachment;

            if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                usageFlags |= resource_usage_flag_bits::DepthStencilAttachment;

            result.insert({ form, format_properties {
                supported,
                (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) == VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,
                properties.linearTilingFeatures != 0
            } });
        }

        return result;
    }
}
