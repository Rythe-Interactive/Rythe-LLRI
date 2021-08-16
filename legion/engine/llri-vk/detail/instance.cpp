/**
 * @file instance.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

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
                validation_callback_source::Implementation,
                callbackData->pMessage,
                desc->userData
            );

            return VK_FALSE;
        }

        void dummyValidationCallback(validation_callback_severity, validation_callback_source, const char*, void*) { }
    }

    namespace detail
    {
        result impl_createInstance(const instance_desc& desc, Instance** instance, bool enableImplementationMessagePolling)
        {
            internal::lazyInitializeVolk();

            auto* result = new Instance();

            std::vector<const char*> layers;
            std::vector<const char*> extensions;

            void* pNext = nullptr;

            //Variables that need to be stored outside of scope
            VkValidationFeaturesEXT features;
            std::vector<VkValidationFeatureEnableEXT> enables;

            for (uint32_t i = 0; i < desc.numExtensions; i++)
            {
                auto& extension = desc.extensions[i];
                switch (extension.type)
                {
                case instance_extension_type::DriverValidation:
                {
                    if (extension.driverValidation.enable)
                        layers.push_back("VK_LAYER_KHRONOS_validation");
                    break;
                }
                case instance_extension_type::GPUValidation:
                {
                    if (extension.gpuValidation.enable)
                    {
                        enables.emplace_back(VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT);
                        enables.emplace_back(VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT);

                        features = VkValidationFeaturesEXT{ VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT, nullptr, (uint32_t)enables.size(), enables.data(), 0, nullptr };
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
            if (enableImplementationMessagePolling && desc.callbackDesc.callback)
            {
                const auto& available = internal::queryAvailableExtensions();
                //Availability of this extension can't be queried externally because API callbacks also include LLRI callbacks
                //so instead the check is implicit, implementation callbacks aren't guaranteed
                if (available.find(internal::nameHash(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) != available.end())
                {
                    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                    result->m_shouldConstructValidationCallbackMessenger = true;
                }
            }

            VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, desc.applicationName, VK_MAKE_VERSION(0, 0, 0), "Legion::LLRI", VK_MAKE_VERSION(0, 0, 1), VK_HEADER_VERSION_COMPLETE };
            VkInstanceCreateInfo instanceCi{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, {}, &appInfo, (uint32_t)layers.size(), layers.data(), (uint32_t)extensions.size(), extensions.data() };
            instanceCi.pNext = pNext;

            VkInstance vulkanInstance = nullptr;
            const VkResult createResult = vkCreateInstance(&instanceCi, nullptr, &vulkanInstance);

            if (createResult != VK_SUCCESS)
            {
                llri::destroyInstance(result);
                return internal::mapVkResult(createResult);
            }
            result->m_ptr = vulkanInstance;

            //Load instance functions 
            volkLoadInstanceOnly(vulkanInstance);

            //Create debug utils callback
            if (result->m_shouldConstructValidationCallbackMessenger)
            {
                result->m_validationCallback = desc.callbackDesc;

                //Attempt to create the debug utils messenger
                if (vkCreateDebugUtilsMessengerEXT) //The extension function may not have been loaded successfully
                {
                    const VkDebugUtilsMessageSeverityFlagsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

                    const VkDebugUtilsMessageTypeFlagsEXT types = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

                    const VkDebugUtilsMessengerCreateInfoEXT debugUtilsCi{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT, nullptr, {}, severity, types, &internal::debugCallback, &result->m_validationCallback };

                    VkDebugUtilsMessengerEXT messenger;
                    vkCreateDebugUtilsMessengerEXT(vulkanInstance, &debugUtilsCi, nullptr, &messenger);

                    result->m_validationCallbackMessenger = messenger;
                }
            }
            else
            {
                result->m_validationCallback = { &internal::dummyValidationCallback, nullptr };
                result->m_validationCallbackMessenger = nullptr;
            }

            *instance = result;
            return result::Success;
        }

        void impl_destroyInstance(Instance* instance)
        {
            if (!instance)
                return;
            const VkInstance vkInstance = static_cast<VkInstance>(instance->m_ptr);

            //Destroy debug messenger if possible
            if (instance->m_validationCallbackMessenger)
            {
                if (vkDestroyDebugUtilsMessengerEXT)
                    vkDestroyDebugUtilsMessengerEXT(vkInstance, static_cast<VkDebugUtilsMessengerEXT>(instance->m_validationCallbackMessenger), nullptr);
            }

            for (auto& [ptr, adapter] : instance->m_cachedAdapters)
                delete adapter;

            //vk validation layers aren't tangible objects and don't need manual destruction

            vkDestroyInstance(vkInstance, nullptr);

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

        std::vector<VkPhysicalDevice> physicalDevices;

        //Get count
        //Can't use Vulkan.hpp convenience function because we need it to not throw if it fails
        uint32_t physicalDeviceCount = 0;
        VkResult r = vkEnumeratePhysicalDevices(static_cast<VkInstance>(m_ptr), &physicalDeviceCount, nullptr);
        if (r != VK_SUCCESS && r != VK_TIMEOUT)
            return internal::mapVkResult(r);

        //Get actual physical devices
        physicalDevices.resize(physicalDeviceCount);
        r = vkEnumeratePhysicalDevices(static_cast<VkInstance>(m_ptr), &physicalDeviceCount, physicalDevices.data());
        if (r != VK_SUCCESS)
            return internal::mapVkResult(r);

        for (VkPhysicalDevice physicalDevice : physicalDevices)
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

        std::vector<VkDeviceQueueCreateInfo> queues;
        float queuePriorities = 1.0f;
        std::vector<const char*> extensions;
        VkPhysicalDeviceFeatures features{};

        //Assign default queue if no queues were selected by the API user //TODO: return invalid api use code when no queues are selected when the queue system is in place
        if (queues.size() == 0)
            queues.push_back(VkDeviceQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, {}, 0, 1, &queuePriorities });

        VkDeviceCreateInfo ci{
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            nullptr,
            {},
            (uint32_t)queues.size(), queues.data(),
            0, nullptr, //Vulkan device layers are deprecated
            (uint32_t)extensions.size(), extensions.data(),
            &features
        };

        VkDevice vkDevice = nullptr;
        const VkResult r = vkCreateDevice(static_cast<VkPhysicalDevice>(desc.adapter->m_ptr), &ci, nullptr, &vkDevice);
        if (r != VK_SUCCESS)
        {
            destroyDevice(output);
            return internal::mapVkResult(r);
        }
        output->m_ptr = vkDevice;

        VolkDeviceTable* table = new VolkDeviceTable();
        volkLoadDeviceTable(table, vkDevice);
        output->m_functionTable = table;

        *device = output;
        return result::Success;
    }

    void Instance::impl_destroyDevice(Device* device) const
    {
        if (!device)
            return;

        if (device->m_ptr)
            static_cast<VolkDeviceTable*>(device->m_functionTable)->vkDestroyDevice(static_cast<VkDevice>(device->m_ptr), nullptr);

        delete static_cast<VolkDeviceTable*>(device->m_functionTable);

        delete device;
    }
}
