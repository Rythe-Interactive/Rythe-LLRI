/**
 * @file adapter.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>
#include <graphics/vulkan/volk.h>

namespace llri
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

        // Set all the information in a structured way here

        *features = result;
        return result::Success;
    }

    adapter_limits Adapter::impl_queryLimits() const
    {
        adapter_limits output{};
        return output;
    }

    bool Adapter::impl_queryExtensionSupport(adapter_extension ext) const
    {
        switch (ext)
        {
            default:
                break;
        }

        return false;
    }

    result Adapter::impl_queryQueueCount(queue_type type, uint8_t* count) const
    {
        // Get queue family info
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
                    // Only the graphics queue has the graphics bit set
                    // it usually also has compute & transfer set, because graphics queue tends to be general purpose
                    if ((p.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
                    {
                        *count = static_cast<uint8_t>(p.queueCount);
                        return result::Success;
                    }
                    break;
                }
                case queue_type::Compute:
                {
                    // Dedicated compute family has no graphics bit but does have a compute bit
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
                    // Dedicated transfer family has no graphics bit, no compute bit, but does have a transfer bit
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

            VkFormatProperties formatProps;
            vkGetPhysicalDeviceFormatProperties(static_cast<VkPhysicalDevice>(m_ptr), vkFormat, &formatProps);
            
            // get supported
            const bool supported = formatProps.optimalTilingFeatures != 0;

            // get flags
            resource_usage_flags usageFlags = resource_usage_flag_bits::None;
            if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT) == VK_FORMAT_FEATURE_TRANSFER_SRC_BIT)
                usageFlags |= resource_usage_flag_bits::TransferSrc;

            if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_DST_BIT) == VK_FORMAT_FEATURE_TRANSFER_DST_BIT)
                usageFlags |= resource_usage_flag_bits::TransferDst;

            if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) == VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)
                usageFlags |= resource_usage_flag_bits::Sampled;

            if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) == VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)
                usageFlags |= resource_usage_flag_bits::ShaderWrite;

            if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)
                usageFlags |= resource_usage_flag_bits::ColorAttachment;

            if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                usageFlags |= resource_usage_flag_bits::DepthStencilAttachment;

            // get sample counts
            VkPhysicalDeviceProperties deviceProps;
            vkGetPhysicalDeviceProperties(static_cast<VkPhysicalDevice>(m_ptr), &deviceProps);
            const VkSampleCountFlags counts = deviceProps.limits.framebufferColorSampleCounts & deviceProps.limits.framebufferDepthSampleCounts;
            const std::unordered_map<sample_count, bool> sampleCounts {
                { sample_count::Count1, counts & VK_SAMPLE_COUNT_1_BIT },
                { sample_count::Count2, counts & VK_SAMPLE_COUNT_2_BIT },
                { sample_count::Count4, counts & VK_SAMPLE_COUNT_4_BIT },
                { sample_count::Count8, counts & VK_SAMPLE_COUNT_8_BIT },
                { sample_count::Count16, counts & VK_SAMPLE_COUNT_16_BIT },
                { sample_count::Count32, counts & VK_SAMPLE_COUNT_32_BIT }
            };

            // get types
            std::unordered_map<resource_type, bool> types {
                { resource_type::Buffer, false },
                { resource_type::Texture1D, true },
                { resource_type::Texture2D, true },
                { resource_type::Texture3D, true }
            };
            
#ifdef __APPLE__
            // molten VK doesnt support non-2D depth textures
            if (form == format::D16UNorm || form == format::D24UNormS8UInt ||
                form == format::D32Float || form == format::D32FloatS8X24UInt)
            {
                types[resource_type::Texture1D] = false;
                types[resource_type::Texture3D] = false;
            }
#endif

            result.insert({ form, format_properties {
                supported,
                types,
                usageFlags,
                sampleCounts,
                formatProps.linearTilingFeatures != 0,
            } });
        }

        return result;
    }
}
