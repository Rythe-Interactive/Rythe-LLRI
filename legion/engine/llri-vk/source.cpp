#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

#include <vector>
#include <map>
#include <vulkan/vulkan.hpp>

namespace legion::graphics::llri
{
    namespace internal
    {
        constexpr adapter_type mapPhysicalDeviceType(const VkPhysicalDeviceType& type)
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

        validation_callback_severity mapSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT sev)
        {
            switch (sev)
            {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                return validation_callback_severity::Error;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                return validation_callback_severity::Warning;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                return validation_callback_severity::Info;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                return validation_callback_severity::Verbose;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                break;
            }

            return validation_callback_severity::Info;
        }

        VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
        {
            const auto desc = (validation_callback_desc*)userData;

            desc->callback(
                mapSeverity(severity),
                validation_callback_source::InternalAPI,
                callbackData->pMessage,
                desc->userData
            );

            return VK_FALSE;
        }
    }

    namespace detail
    {
        result impl_createInstance(const instance_desc& desc, Instance** instance, const bool& enableInternalAPIMessagePolling)
        {
            auto* result = new Instance();

            std::vector<const char*> layers;
            std::vector<const char*> extensions;

            void* pNext = nullptr;

            //Variables that need to be stored outside of scope
            vk::ValidationFeaturesEXT features;
            std::vector<vk::ValidationFeatureEnableEXT> enables;

            for (uint32_t i = 0; i < desc.numExtensions; i++)
            {
                auto& extension = desc.extensions[i];
                switch (extension.type)
                {
                    case instance_extension_type::APIValidation:
                    {
                        if (extension.apiValidation.enable)
                            layers.push_back("VK_LAYER_KHRONOS_validation");
                        break;
                    }
                    case instance_extension_type::GPUValidation:
                    {
                        if (extension.gpuValidation.enable)
                        {
                            enables = {
                                vk::ValidationFeatureEnableEXT::eGpuAssisted,
                                vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot
                            };
                            features = vk::ValidationFeaturesEXT(enables, {});
                            features.pNext = pNext; //Always apply pNext backwards to simplify optional chaining
                            pNext = &features;
                        }
                        break;
                    }
                    default:
                    {
                        if (desc.callbackDesc.callback)
                            desc.callbackDesc(validation_callback_severity::Error, validation_callback_source::Validation, (std::string("createInstance() returned ErrorExtensionNotSupported because the extension type ") + std::to_string((int)extension.type) + " is not recognized.").c_str());

                        llri::destroyInstance(result);
                        return result::ErrorExtensionNotSupported;
                    }
                }
            }

            //Add the debug utils extension for the API callback
            result->m_shouldConstructValidationCallbackMessenger = false;
            result->m_validationCallbackMessenger = nullptr;
            if (enableInternalAPIMessagePolling && desc.callbackDesc.callback)
            {
                auto available = internal::queryAvailableExtensions();
                //Availability of this extension can't be queried externally because API callbacks also include LLRI callbacks
                //so instead the check is implicit, internal API callbacks aren't guaranteed
                if (available.find(internal::nameHash(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) != available.end())
                {
                    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                    result->m_shouldConstructValidationCallbackMessenger = true;
                }
            }

            vk::ApplicationInfo appInfo{ desc.applicationName, VK_MAKE_VERSION(0, 0, 0), "Legion::LLRI", VK_MAKE_VERSION(0, 0, 1), VK_HEADER_VERSION_COMPLETE };
            vk::InstanceCreateInfo instanceCi{ {}, &appInfo, (uint32_t)layers.size(), layers.data(), (uint32_t)extensions.size(), extensions.data() };
            instanceCi.pNext = pNext;

            vk::Instance vulkanInstance = nullptr;
            const vk::Result createResult = vk::createInstance(&instanceCi, nullptr, &vulkanInstance);

            if (createResult != vk::Result::eSuccess)
            {
                llri::destroyInstance(result);
                return internal::mapVkResult(createResult);
            }
            result->m_ptr = vulkanInstance;

            //Create debug utils callback
            if (result->m_shouldConstructValidationCallbackMessenger)
            {
                result->m_validationCallback = desc.callbackDesc;

                vk::DebugUtilsMessageSeverityFlagsEXT severity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

                vk::DebugUtilsMessageTypeFlagsEXT types = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

                const vk::DebugUtilsMessengerCreateInfoEXT debugUtilsCi{ {}, severity, types, &internal::debugCallback, &result->m_validationCallback };

                const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vulkanInstance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
                if (func)
                {
                    auto vkCi = static_cast<VkDebugUtilsMessengerCreateInfoEXT>(debugUtilsCi);
                    VkDebugUtilsMessengerEXT messenger;
                    func(static_cast<VkInstance>(vulkanInstance), &vkCi, nullptr, &messenger);

                    result->m_validationCallbackMessenger = messenger;
                }
            }

            *instance = result;
            return result::Success;
        }

        void impl_destroyInstance(Instance* instance)
        {
            if (!instance)
                return;
            const vk::Instance vkInstance = static_cast<VkInstance>(instance->m_ptr);

            //Destroy debug messenger if possible
            if (instance->m_validationCallbackMessenger)
            {
                const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkInstance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
                if (func != nullptr)
                    func(static_cast<VkInstance>(vkInstance), static_cast<VkDebugUtilsMessengerEXT>(instance->m_validationCallbackMessenger), nullptr);
            }

            for (auto& [ptr, adapter] : instance->m_cachedAdapters)
                delete adapter;

            //vk validation layers aren't tangible objects and don't need manual destruction

            delete instance;
        }

        void impl_pollAPIMessages(const validation_callback_desc& validation, messenger_type* messenger)
        {
            //Empty because vulkan uses a callback system
            //suppress unused parameter warnings
            (void)validation;
            (void)messenger;
        }
    }

    result Instance::impl_enumerateAdapters(std::vector<Adapter*>* adapters)
    {
        adapters->clear();

        //Clear internal pointers, lost adapters will have a nullptr internally
        for (auto& [ptr, adapter] : m_cachedAdapters)
            adapter->m_ptr = nullptr;

        std::vector<vk::PhysicalDevice> physicalDevices;

        //Get count
        //Can't use Vulkan.hpp convenience function because we need it to not throw if it fails
        uint32_t physicalDeviceCount = 0;
        vk::Result r = static_cast<vk::Instance>(static_cast<VkInstance>(m_ptr)).enumeratePhysicalDevices(&physicalDeviceCount, nullptr);
        if (r != vk::Result::eSuccess && r != vk::Result::eTimeout)
            return internal::mapVkResult(r);

        //Get actual physical devices
        physicalDevices.resize(physicalDeviceCount);
        r = static_cast<vk::Instance>(static_cast<VkInstance>(m_ptr)).enumeratePhysicalDevices(&physicalDeviceCount, physicalDevices.data());
        if (r != vk::Result::eSuccess)
            return internal::mapVkResult(r);

        for (vk::PhysicalDevice physicalDevice : physicalDevices)
        {
            if (m_cachedAdapters.find(physicalDevice) != m_cachedAdapters.end())
            {
                //Re-assign pointer to found adapters
                m_cachedAdapters[physicalDevice]->m_ptr = physicalDevice;
                adapters->push_back(m_cachedAdapters[physicalDevice]);
            }
            else
            {
                Adapter* adapter = new Adapter();
                adapter->m_ptr = physicalDevice;
                adapter->m_validationCallback = m_validationCallback;

                m_cachedAdapters[physicalDevice] = adapter;
                adapters->push_back(adapter);
            }
        }

        return result::Success;
    }

    result Instance::impl_createDevice(const device_desc& desc, Device** device) const
    {
        Device* output = new Device();
        output->m_validationCallback = m_validationCallback;

        std::vector<vk::DeviceQueueCreateInfo> queues;
        float queuePriorities = 1.0f;
        std::vector<const char*> extensions;
        vk::PhysicalDeviceFeatures features{};

        //Assign default queue if no queues were selected by the API user //TODO: return invalid api use code when no queues are selected when the queue system is in place
        if (queues.size() == 0)
            queues.push_back(vk::DeviceQueueCreateInfo{ {}, 0, 1, &queuePriorities });

        vk::DeviceCreateInfo ci{
            {},
            (uint32_t)queues.size(), queues.data(),
            0, nullptr, //Vulkan device layers are deprecated
            (uint32_t)extensions.size(), extensions.data(),
            &features
        };

        vk::Device vkDevice = nullptr;
        const vk::Result r = static_cast<vk::PhysicalDevice>(static_cast<VkPhysicalDevice>(desc.adapter->m_ptr)).createDevice(&ci, nullptr, &vkDevice);
        if (r != vk::Result::eSuccess)
        {
            destroyDevice(output);
            return internal::mapVkResult(r);
        }
        output->m_ptr = vkDevice;

        *device = output;
        return result::Success;
    }

    void Instance::impl_destroyDevice(Device* device) const
    {
        if (device->m_ptr != nullptr)
            vkDestroyDevice(static_cast<VkDevice>(device->m_ptr), nullptr);

        delete device;
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

    result Adapter::impl_queryExtensionSupport(const adapter_extension_type& type, bool* supported) const
    {
        *supported = false;

        switch (type)
        {
        default:
            break;
        }

        return result::Success;
    }
}
