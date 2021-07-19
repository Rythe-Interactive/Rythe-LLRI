#include <llri/llri.hpp>

#include <vector>
#include <map>
#include <vulkan/vulkan.hpp>

namespace legion::graphics::llri
{
    namespace internal
    {
        std::map<std::string, vk::LayerProperties>& queryAvailableLayers();
        std::map<std::string, vk::ExtensionProperties>& queryAvailableExtensions();
        result mapVkResult(const vk::Result& result);
    }

    result createInstance(const instance_desc& desc, Instance* instance)
    {
        if (instance == nullptr)
            return result::ErrorInvalidUsage;
        if (desc.numExtensions > 0 && desc.extensions == nullptr)
            return result::ErrorInvalidUsage;

        auto* result = new InstanceT();

        std::vector<const char*> layers;
        std::vector<const char*> extensions;

        auto availableLayers = internal::queryAvailableLayers();
        auto availableExtensions = internal::queryAvailableExtensions();

        void* pNext = nullptr;

        //Variables that need to be stored outside of scope
        vk::ValidationFeaturesEXT features; 
        std::vector<vk::ValidationFeatureEnableEXT> enables;

        for (uint32_t i = 0; i < desc.numExtensions; i++)
        {
            auto& extension = desc.extensions[i];
            switch(extension.type)
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
                    llri::destroyInstance(result);
                    return result::ErrorExtensionNotSupported;
                }
            }
        }

        vk::ApplicationInfo appInfo{ desc.applicationName, VK_MAKE_VERSION(0, 0, 0), "Legion::LLRI", VK_MAKE_VERSION(0, 0, 1), VK_HEADER_VERSION_COMPLETE };
        vk::InstanceCreateInfo ci{ {}, &appInfo, (uint32_t)layers.size(), layers.data(), (uint32_t)extensions.size(), extensions.data() };
        ci.pNext = pNext;

        vk::Instance vulkanInstance = nullptr;
        const vk::Result createResult = vk::createInstance(&ci, nullptr, &vulkanInstance);

        if (createResult != vk::Result::eSuccess)
        {
            llri::destroyInstance(result);
            return internal::mapVkResult(createResult);
        }

        result->m_ptr = vulkanInstance;
        *instance = result;
        return result::Success;
    }

    void destroyInstance(Instance instance)
    {
        if (!instance)
            return;

        for (auto& [ptr, adapter] : instance->m_cachedAdapters)
            delete adapter;

        //vk validation layers aren't tangible objects and don't need manual destruction

        delete instance;
    }

    result InstanceT::enumerateAdapters(std::vector<Adapter>* adapters)
    {
        if (adapters == nullptr)
            return result::ErrorInvalidUsage;

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
                Adapter adapter = new AdapterT();
                adapter->m_ptr = physicalDevice;

                m_cachedAdapters[physicalDevice] = adapter;
                adapters->push_back(adapter);
            }
        }

        return result::Success;
    }

    result InstanceT::createDevice(const device_desc& desc, Device* device)
    {
        if (m_ptr == nullptr || device == nullptr || desc.adapter == nullptr)
            return result::ErrorInvalidUsage;

        if (desc.numExtensions > 0 && desc.extensions == nullptr)
            return result::ErrorInvalidUsage;

        if (desc.adapter->m_ptr == nullptr)
            return result::ErrorDeviceLost;

        Device result = new DeviceT();

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
            destroyDevice(result);
            return internal::mapVkResult(r);
        }
        result->m_ptr = vkDevice;

        *device = result;
        return result::Success;
    }

    void InstanceT::destroyDevice(Device device)
    {
        if (device->m_ptr != nullptr)
            vkDestroyDevice(static_cast<VkDevice>(device->m_ptr), nullptr);

        delete device;
    }

    constexpr adapter_type mapPhysicalDeviceType(const VkPhysicalDeviceType& type)
    {
        switch(type)
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

    result AdapterT::queryInfo(adapter_info* info) const
    {
        if (info == nullptr)
            return result::ErrorInvalidUsage;

        if (m_ptr == nullptr)
            return result::ErrorDeviceRemoved;

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(static_cast<VkPhysicalDevice>(m_ptr), &properties);

        adapter_info result{};
        result.vendorId = properties.vendorID;
        result.adapterId = properties.deviceID;
        result.adapterName = properties.deviceName;
        result.adapterType = mapPhysicalDeviceType(properties.deviceType);
        *info = result;
        return result::Success;
    }

    result AdapterT::queryFeatures(adapter_features* features) const
    {
        if (features == nullptr)
            return result::ErrorInvalidUsage;

        if (m_ptr == nullptr)
            return result::ErrorDeviceRemoved;

        VkPhysicalDeviceFeatures physicalFeatures;
        vkGetPhysicalDeviceFeatures(static_cast<VkPhysicalDevice>(m_ptr), &physicalFeatures);

        adapter_features result;

        //Set all the information in a structured way here

        *features = result;
        return result::Success;
    }

    bool AdapterT::queryExtensionSupport(const adapter_extension_type& type) const
    {
        switch(type)
        {
            default:
                break;
        }

        return false;
    }
}
