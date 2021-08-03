#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

namespace legion::graphics::llri
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
        result mapVkResult(const VkResult& result)
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
                case VK_ERROR_FORMAT_NOT_SUPPORTED: break;
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
    }
}