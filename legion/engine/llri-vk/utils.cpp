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
            layer_map result;

            std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
            for (auto layer : availableLayers)
                result.emplace(nameHash(layer.layerName.data()), layer);

            return result;
        }

        const layer_map& queryAvailableLayers()
        {
            static layer_map availableLayers = preprocessLayerProperties();
            return availableLayers;
        }

        extension_map preprocessExtensionProperties()
        {
            extension_map result;

            std::vector<vk::ExtensionProperties> availableExtensions = vk::enumerateInstanceExtensionProperties();
            for (auto extension : availableExtensions)
                result.emplace(nameHash(extension.extensionName.data()), extension);

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
        result mapVkResult(const vk::Result& result)
        {
            switch (result)
            {
                case vk::Result::eSuccess:
                    return result::Success;
                case vk::Result::eNotReady:
                    return result::NotReady;
                case vk::Result::eTimeout:
                    return result::Timeout;
                case vk::Result::eEventSet: break;
                case vk::Result::eEventReset: break;
                case vk::Result::eIncomplete: break;
                case vk::Result::eErrorOutOfHostMemory:
                    return result::ErrorOutOfHostMemory;
                case vk::Result::eErrorOutOfDeviceMemory:
                    return result::ErrorOutOfDeviceMemory;
                case vk::Result::eErrorInitializationFailed:
                    return result::ErrorInitializationFailed;
                case vk::Result::eErrorDeviceLost:
                    return result::ErrorDeviceLost;
                case vk::Result::eErrorMemoryMapFailed: break;
                case vk::Result::eErrorLayerNotPresent:
                    return result::ErrorExtensionNotSupported;
                case vk::Result::eErrorExtensionNotPresent:
                    return result::ErrorExtensionNotSupported;
                case vk::Result::eErrorFeatureNotPresent:
                    return result::ErrorFeatureNotSupported;
                case vk::Result::eErrorIncompatibleDriver:
                    return result::ErrorIncompatibleDriver;
                case vk::Result::eErrorTooManyObjects: break;
                case vk::Result::eErrorFormatNotSupported: break;
                case vk::Result::eErrorFragmentedPool: break;
                case vk::Result::eErrorUnknown:
                    return result::ErrorUnknown;
                case vk::Result::eErrorOutOfPoolMemory: break;
                case vk::Result::eErrorInvalidExternalHandle: break;
                case vk::Result::eErrorFragmentation: break;
                case vk::Result::eErrorInvalidOpaqueCaptureAddress: break;
                case vk::Result::eErrorSurfaceLostKHR: break;
                case vk::Result::eErrorNativeWindowInUseKHR: break;
                case vk::Result::eSuboptimalKHR: break;
                case vk::Result::eErrorOutOfDateKHR: break;
                case vk::Result::eErrorIncompatibleDisplayKHR: break;
                case vk::Result::eErrorValidationFailedEXT: break;
                case vk::Result::eErrorInvalidShaderNV: break;
                case vk::Result::eErrorInvalidDrmFormatModifierPlaneLayoutEXT: break;
                case vk::Result::eErrorNotPermittedEXT: break;
                case vk::Result::eThreadIdleKHR: break;
                case vk::Result::eThreadDoneKHR: break;
                case vk::Result::eOperationDeferredKHR: break;
                case vk::Result::eOperationNotDeferredKHR: break;
                case vk::Result::ePipelineCompileRequiredEXT: break;
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
