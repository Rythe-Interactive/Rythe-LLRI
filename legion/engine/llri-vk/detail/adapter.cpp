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
        *supported = false;

        switch (type)
        {
        default:
            break;
        }

        return result::Success;
    }

    result Adapter::impl_queryQueueCount(queue_type type, uint8_t* count) const
    {
        *count = 0;

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

    result Adapter::impl_queryNodeCountEXT(uint8_t* count) const
    {
        uint32_t groupCount;
        vkEnumeratePhysicalDeviceGroups(static_cast<VkInstance>(m_instanceHandle), &groupCount, nullptr);

        std::vector<VkPhysicalDeviceGroupProperties> groups(groupCount);
        vkEnumeratePhysicalDeviceGroups(static_cast<VkInstance>(m_instanceHandle), &groupCount, groups.data());

        for (const VkPhysicalDeviceGroupProperties& group : groups)
        {
            for (uint32_t i = 0; i < group.physicalDeviceCount; i++)
            {
                if (group.physicalDevices[i] == static_cast<VkPhysicalDevice>(m_ptr))
                {
                    *count = static_cast<uint8_t>(group.physicalDeviceCount);
                    return result::Success;
                }
            }
        }

        //This should never happen because every physical device is part of a device group
        return result::ErrorUnknown;
    }
}
