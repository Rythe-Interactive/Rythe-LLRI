/**
 * @file utils.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

namespace LLRI_NAMESPACE
{
    namespace internal
    {
        /**
         * @brief Helper function that maps layers to their names
        */
        layer_map preprocessLayerProperties()
        {
            lazyInitializeVolk();

            layer_map result;

            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
            std::vector<VkLayerProperties> layers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

            for (auto layer : layers)
                result.emplace(nameHash(layer.layerName), layer);

            return result;
        }

        const layer_map& queryAvailableLayers()
        {
            static layer_map availableLayers = preprocessLayerProperties();
            return availableLayers;
        }

        extension_map preprocessExtensionProperties()
        {
            lazyInitializeVolk();

            extension_map result;

            uint32_t extensionCount;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> extensions(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

            for (auto extension : extensions)
                result.emplace(nameHash(extension.extensionName), extension);

            return result;
        }

        /**
         * @brief Helps prevent loading in extensions multiple times by loading them in as a static variable in this function
        */
        const extension_map& queryAvailableExtensions()
        {
            static extension_map availableExtensions = preprocessExtensionProperties();
            return availableExtensions;
        }

        /**
         * @brief Helper function that converts vk::Result to llri::result
        */
        result mapVkResult(VkResult result)
        {
            switch (result)
            {
                case VK_SUCCESS:
                    return result::Success;
                case VK_NOT_READY:
                    return result::NotReady;
                case VK_TIMEOUT:
                    return result::Timeout;
                case VK_EVENT_SET: break;
                case VK_EVENT_RESET: break;
                case VK_INCOMPLETE: break;
                case VK_ERROR_OUT_OF_HOST_MEMORY:
                    return result::ErrorOutOfHostMemory;
                case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                    return result::ErrorOutOfDeviceMemory;
                case VK_ERROR_INITIALIZATION_FAILED:
                    return result::ErrorInitializationFailed;
                case VK_ERROR_DEVICE_LOST:
                    return result::ErrorDeviceLost;
                case VK_ERROR_MEMORY_MAP_FAILED: break;
                case VK_ERROR_LAYER_NOT_PRESENT:
                    return result::ErrorExtensionNotSupported;
                case VK_ERROR_EXTENSION_NOT_PRESENT:
                    return result::ErrorExtensionNotSupported;
                case VK_ERROR_FEATURE_NOT_PRESENT:
                    return result::ErrorFeatureNotSupported;
                case VK_ERROR_INCOMPATIBLE_DRIVER:
                    return result::ErrorIncompatibleDriver;
                case VK_ERROR_TOO_MANY_OBJECTS: break;
                case VK_ERROR_FORMAT_NOT_SUPPORTED:
                    return result::ErrorInvalidFormat;
                case VK_ERROR_FRAGMENTED_POOL: break;
                case VK_ERROR_UNKNOWN:
                    return result::ErrorUnknown;
                case VK_ERROR_OUT_OF_POOL_MEMORY: break;
                case VK_ERROR_INVALID_EXTERNAL_HANDLE: break;
                case VK_ERROR_FRAGMENTATION: break;
                case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: break;
                case VK_ERROR_SURFACE_LOST_KHR: break;
                case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: break;
                case VK_SUBOPTIMAL_KHR: break;
                case VK_ERROR_OUT_OF_DATE_KHR: break;
                case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: break;
                case VK_ERROR_VALIDATION_FAILED_EXT: break;
                case VK_ERROR_INVALID_SHADER_NV: break;
                case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: break;
                case VK_ERROR_NOT_PERMITTED_EXT: break;
                case VK_THREAD_IDLE_KHR: break;
                case VK_THREAD_DONE_KHR: break;
                case VK_OPERATION_DEFERRED_KHR: break;
                case VK_OPERATION_NOT_DEFERRED_KHR: break;
                case VK_PIPELINE_COMPILE_REQUIRED_EXT: break;
                default: break;
            }

            return result::ErrorUnknown;
        }

        uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t requiredMemoryBits, VkMemoryPropertyFlags requiredFlags)
        {
            VkPhysicalDeviceMemoryProperties properties;
            vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);

            for (size_t i = 0; i < properties.memoryTypeCount; i++)
            {
                const uint32_t bits = 1 << i;
                const bool isRequiredMemoryType = requiredMemoryBits & bits;

                const VkMemoryPropertyFlags flags = properties.memoryTypes[i].propertyFlags;
                const bool hasRequiredProperties = (flags & requiredFlags) == requiredFlags;

                if (isRequiredMemoryType && hasRequiredProperties)
                    return static_cast<uint32_t>(i);
            }

            return static_cast<uint32_t>(-1);
        }

        /**
         * @brief Utility function for hashing strings for layer/extension names
        */
        unsigned long long nameHash(std::string name)
        {
            static std::hash<std::string> hasher{};
            if (!name.empty() && name[name.size() - 1] == '\0')
            {
                std::string temp = name;
                temp.resize(name.size() - 1);
                return hasher(temp);
            }
            return hasher(name);
        }

        std::map<queue_type, uint32_t> findQueueFamilies(VkPhysicalDevice physicalDevice)
        {
            std::map<queue_type, uint32_t> output
            {
                { queue_type::Graphics, UINT_MAX },
                { queue_type::Compute, UINT_MAX },
                { queue_type::Transfer, UINT_MAX }
            };

            //Get queue family info
            uint32_t propertyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propertyCount, nullptr);
            std::vector<VkQueueFamilyProperties> properties(propertyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &propertyCount, properties.data());

            for (size_t i = 0; i < propertyCount; i++)
            {
                auto& p = properties[i];

                //Only the graphics queue has the graphics bit set
                //it usually also has compute & transfer set, because graphics queue tends to be general purpose
                if ((p.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT)
                    output[queue_type::Graphics] = i;

                //Dedicated compute family has no graphics bit but does have a compute bit
                else if ((p.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 && (p.queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT)
                    output[queue_type::Compute] = i;

                //Dedicated transfer family has no graphics bit, no compute bit, but does have a transfer bit
                else if ((p.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
                    (p.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0 &&
                    (p.queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT)
                    output[queue_type::Transfer] = i;
            }

            return output;
        }
    }
}
