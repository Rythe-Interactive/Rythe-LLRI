#include <llri/llri.hpp>
#include <vulkan/vulkan.hpp>
#include <map>
#include <string>
#include <vector>

namespace legion::graphics::llri
{
    namespace internal
    {
        /**
         * @brief Helper function that maps layers to their names
        */
        std::map<std::string, vk::LayerProperties> preprocessLayerProperties()
        {
            std::map<std::string, vk::LayerProperties> result;

            std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();
            for (auto layer : availableLayers)
                result[layer.layerName.data()] = layer;

            return result;
        }

        /**
         * @brief Helps prevent loading in layers multiple times by loading them in as a static variable in this function
        */
        std::map<std::string, vk::LayerProperties>& queryAvailableLayers()
        {
            static std::map<std::string, vk::LayerProperties> availableLayers = preprocessLayerProperties();
            return availableLayers;
        }

        /**
         * @brief Helper function that maps extensions to their names
        */
        std::map<std::string, vk::ExtensionProperties> preprocessExtensionProperties()
        {
            std::map<std::string, vk::ExtensionProperties> result;

            std::vector<vk::ExtensionProperties> availableExtensions = vk::enumerateInstanceExtensionProperties();
            for (auto extension : availableExtensions)
                result[extension.extensionName] = extension;

            return result;
        }

        /**
         * @brief Helps prevent loading in extensions multiple times by loading them in as a static variable in this function
        */
        std::map<std::string, vk::ExtensionProperties>& queryAvailableExtensions()
        {
            static std::map<std::string, vk::ExtensionProperties> availableExtensions = preprocessExtensionProperties();
            return availableExtensions;
        }

        /**
         * @brief Helper function that converts vk::Result to llri::Result
        */
        Result mapVkResult(const vk::Result& result)
        {
            switch (result)
            {
                case vk::Result::eSuccess:
                    return Result::Success;
                case vk::Result::eNotReady:
                    return Result::NotReady;
                case vk::Result::eTimeout:
                    return Result::Timeout;
                case vk::Result::eEventSet: break;
                case vk::Result::eEventReset: break;
                case vk::Result::eIncomplete: break;
                case vk::Result::eErrorOutOfHostMemory:
                    return Result::ErrorOutOfHostMemory;
                case vk::Result::eErrorOutOfDeviceMemory:
                    return Result::ErrorOutOfDeviceMemory;
                case vk::Result::eErrorInitializationFailed:
                    return Result::ErrorInitializationFailed;
                case vk::Result::eErrorDeviceLost:
                    return Result::ErrorDeviceLost;
                case vk::Result::eErrorMemoryMapFailed: break;
                case vk::Result::eErrorLayerNotPresent:
                    return Result::ErrorExtensionNotSupported;
                case vk::Result::eErrorExtensionNotPresent:
                    return Result::ErrorExtensionNotSupported;
                case vk::Result::eErrorFeatureNotPresent:
                    return Result::ErrorFeatureNotSupported;
                case vk::Result::eErrorIncompatibleDriver:
                    return Result::ErrorIncompatibleDriver;
                case vk::Result::eErrorTooManyObjects: break;
                case vk::Result::eErrorFormatNotSupported: break;
                case vk::Result::eErrorFragmentedPool: break;
                case vk::Result::eErrorUnknown:
                    return Result::ErrorUnknown;
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

            return Result::ErrorUnknown;
        }
    }
}
