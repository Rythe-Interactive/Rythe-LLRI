/**
 * @file instance.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>
#include <algorithm>

namespace llri
{
    namespace detail
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

        message_severity mapSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT sev)
        {
            switch (sev)
            {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                return message_severity::Error;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                return message_severity::Warning;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                return message_severity::Info;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                return message_severity::Verbose;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
                break;
            }

            return message_severity::Info;
        }

        VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                               [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT type,
                               const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                               [[maybe_unused]] void* userData)
        {
            detail::callUserCallback(mapSeverity(severity), message_source::Implementation, callbackData->pMessage);
            return VK_FALSE;
        }
    }

    namespace detail
    {
        result impl_createInstance(const instance_desc& desc, Instance** instance, bool enableImplementationMessagePolling)
        {
            detail::lazyInitializeVolk();

            auto* output = new Instance();
            output->m_desc = desc;
            const auto& availableExtensions = detail::queryAvailableExtensions();

            std::vector<const char*> layers;
            std::unordered_map<uint64_t, const char*> extensions;
            if (availableExtensions.find(detail::nameHash("VK_KHR_device_group_creation")) != availableExtensions.end())
                extensions.emplace(detail::nameHash("VK_KHR_device_group_creation"), "VK_KHR_device_group_creation");

#ifdef __APPLE__
            // if available, enable - necessary for MoltenVK
            if (availableExtensions.find(detail::nameHash("VK_KHR_get_physical_device_properties2")) != availableExtensions.end())
                extensions.emplace(detail::nameHash("VK_KHR_get_physical_device_properties2"), "VK_KHR_get_physical_device_properties2");
#endif
            
            void* pNext = nullptr;

            // Variables that need to be stored outside of scope
            VkValidationFeaturesEXT features;
            std::vector<VkValidationFeatureEnableEXT> enables;

            for (size_t i = 0; i < desc.numExtensions; i++)
            {
                auto& extension = desc.extensions[i];
                switch (extension)
                {
                    case instance_extension::DriverValidation:
                    {
                        layers.emplace_back("VK_LAYER_KHRONOS_validation");
                        break;
                    }
                    case instance_extension::GPUValidation:
                    {
                        extensions.emplace(detail::nameHash("VK_EXT_validation_features"), "VK_EXT_validation_features");

                        enables.emplace_back(VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT);
                        enables.emplace_back(VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT);

                        features = VkValidationFeaturesEXT{ VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT, nullptr, static_cast<uint32_t>(enables.size()), enables.data(), 0, nullptr };
                        features.pNext = pNext; // Always apply pNext backwards to simplify optional chaining
                        pNext = &features;
                        break;
                    }
                    case instance_extension::SurfaceWin32:
                    {
                        extensions.emplace(detail::nameHash("VK_KHR_surface"), "VK_KHR_surface");
                        extensions.emplace(detail::nameHash("VK_KHR_win32_surface"), "VK_KHR_win32_surface");
                        break;
                    }
                    case instance_extension::SurfaceCocoa:
                    {
                        extensions.emplace(detail::nameHash("VK_KHR_surface"), "VK_KHR_surface");
                        extensions.emplace(detail::nameHash("VK_EXT_metal_surface"), "VK_EXT_metal_surface");
                        break;
                    }
                    case instance_extension::SurfaceXlib:
                    {
                        extensions.emplace(detail::nameHash("VK_KHR_surface"), "VK_KHR_surface");
                        extensions.emplace(detail::nameHash("VK_KHR_xlib_surface"), "VK_KHR_xlib_surface");
                        break;
                    }
                    case instance_extension::SurfaceXcb:
                    {
                        extensions.emplace(detail::nameHash("VK_KHR_surface"), "VK_KHR_surface");
                        extensions.emplace(detail::nameHash("VK_KHR_xcb_surface"), "VK_KHR_xcb_surface");
                        break;
                    }
                }
            }

            // Add the debug utils extension for the API callback
            output->m_shouldConstructValidationCallbackMessenger = false;
            output->m_validationCallbackMessenger = nullptr;
            if (enableImplementationMessagePolling)
            {
                // Availability of this extension can't be queried externally because API callbacks also include LLRI callbacks
                // so instead the check is implicit, implementation callbacks aren't guaranteed
                if (availableExtensions.find(detail::nameHash("VK_EXT_debug_utils")) != availableExtensions.end())
                {
                    extensions.emplace(detail::nameHash(VK_EXT_DEBUG_UTILS_EXTENSION_NAME), VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                    output->m_shouldConstructValidationCallbackMessenger = true;
                }
            }

            // transfer over to vector of strings
            std::vector<const char*> extensionVec{extensions.size()};
            std::transform(extensions.begin(), extensions.end(), extensionVec.begin(), [](auto& pair){ return pair.second;});

            VkApplicationInfo appInfo{ VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, desc.applicationName, VK_MAKE_VERSION(0, 0, 0), "Rythe::LLRI", VK_MAKE_VERSION(0, 0, 1), VK_HEADER_VERSION_COMPLETE };
            VkInstanceCreateInfo instanceCi{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, {}, &appInfo, static_cast<uint32_t>(layers.size()), layers.data(), static_cast<uint32_t>(extensionVec.size()), extensionVec.data() };
            instanceCi.pNext = pNext;

            VkInstance vulkanInstance = nullptr;
            const VkResult createResult = vkCreateInstance(&instanceCi, nullptr, &vulkanInstance);

            if (createResult != VK_SUCCESS)
            {
                llri::destroyInstance(output);
                return detail::mapVkResult(createResult);
            }
            output->m_ptr = vulkanInstance;

            // Load instance functions 
            volkLoadInstanceOnly(vulkanInstance);

            // Create debug utils callback
            if (output->m_shouldConstructValidationCallbackMessenger)
            {
                // Attempt to create the debug utils messenger
                if (vkCreateDebugUtilsMessengerEXT) // The extension function may not have been loaded successfully
                {
                    const VkDebugUtilsMessageSeverityFlagsEXT severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

                    const VkDebugUtilsMessageTypeFlagsEXT types = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

                    const VkDebugUtilsMessengerCreateInfoEXT debugUtilsCi{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT, nullptr, {}, severity, types, &detail::debugCallback, nullptr };

                    VkDebugUtilsMessengerEXT messenger;
                    vkCreateDebugUtilsMessengerEXT(vulkanInstance, &debugUtilsCi, nullptr, &messenger);

                    output->m_validationCallbackMessenger = messenger;
                }
            }
            else
            {
                output->m_validationCallbackMessenger = nullptr;
            }

            *instance = output;
            return result::Success;
        }

        void impl_destroyInstance(Instance* instance)
        {
            const VkInstance vkInstance = static_cast<VkInstance>(instance->m_ptr);

            // Destroy debug messenger if possible
            if (instance->m_validationCallbackMessenger)
            {
                if (vkDestroyDebugUtilsMessengerEXT)
                    vkDestroyDebugUtilsMessengerEXT(vkInstance, static_cast<VkDebugUtilsMessengerEXT>(instance->m_validationCallbackMessenger), nullptr);
            }

            for (auto& [ptr, adapter] : instance->m_cachedAdapters)
                delete adapter;

            // vk validation layers aren't tangible objects and don't need manual destruction

            if (vkInstance)
                vkDestroyInstance(vkInstance, nullptr);

            delete instance;
        }

        void impl_pollAPIMessages([[maybe_unused]] messenger_type* messenger)
        {
            // Empty because vulkan uses a callback system
        }
    }

    result Instance::impl_enumerateAdapters(std::vector<Adapter*>* adapters)
    {
        const auto& extensions = detail::queryAvailableExtensions();
        const bool groupsSupported = extensions.find(detail::nameHash("VK_KHR_device_group_creation")) != extensions.end();

        std::vector<VkPhysicalDevice> physicalDevices;

        // Ideally we iterate over physical devices using device groups
        // so that linked adapters are listed as single adapters
        if (groupsSupported)
        {
            uint32_t groupCount;
            VkResult r = vkEnumeratePhysicalDeviceGroups(static_cast<VkInstance>(m_ptr), &groupCount, nullptr);
            if (r != VK_SUCCESS)
                return detail::mapVkResult(r);

            std::vector<VkPhysicalDeviceGroupProperties> groups(groupCount);
            r = vkEnumeratePhysicalDeviceGroups(static_cast<VkInstance>(m_ptr), &groupCount, groups.data());
            if (r != VK_SUCCESS)
                return detail::mapVkResult(r);

            for (const auto& group : groups)
            {
                // First try to find existing physical devices and re-assign pointer to found adapters
                // It is possible that physial device [0] is not the one stored so we iterate over all and find the related one
                bool found = false;
                for (size_t i = 0; i < group.physicalDeviceCount; i++)
                {
                    if (m_cachedAdapters.find(group.physicalDevices[i]) != m_cachedAdapters.end())
                    {
                        m_cachedAdapters[group.physicalDevices[i]]->m_ptr = group.physicalDevices[i];
                        m_cachedAdapters[group.physicalDevices[i]]->m_nodeCount = static_cast<uint8_t>(group.physicalDeviceCount);
                        adapters->push_back(m_cachedAdapters[group.physicalDevices[i]]);
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    Adapter* adapter = new Adapter();
                    adapter->m_ptr = group.physicalDevices[0];
                    adapter->m_instance = this;
                    adapter->m_nodeCount = static_cast<uint8_t>(group.physicalDeviceCount);

                    m_cachedAdapters[group.physicalDevices[0]] = adapter;
                    adapters->push_back(adapter);
                }
            }
        }
        else
        {            
            uint32_t physicalDeviceCount = 0;
            VkResult r = vkEnumeratePhysicalDevices(static_cast<VkInstance>(m_ptr), &physicalDeviceCount, nullptr);
            if (r != VK_SUCCESS)
                return detail::mapVkResult(r);

            // Get actual physical devices
            physicalDevices.resize(physicalDeviceCount);
            r = vkEnumeratePhysicalDevices(static_cast<VkInstance>(m_ptr), &physicalDeviceCount, physicalDevices.data());
            if (r != VK_SUCCESS)
                return detail::mapVkResult(r);
           
            for (VkPhysicalDevice physicalDevice : physicalDevices)
            {
                if (m_cachedAdapters.find(physicalDevice) != m_cachedAdapters.end())
                {
                    // Re-assign pointer to found adapters
                    m_cachedAdapters[physicalDevice]->m_ptr = physicalDevice;
                    adapters->push_back(m_cachedAdapters[physicalDevice]);
                }
                else
                {
                    Adapter* adapter = new Adapter();
                    adapter->m_ptr = physicalDevice;
                    adapter->m_instance = this;

                    m_cachedAdapters[physicalDevice] = adapter;
                    adapters->push_back(adapter);
                }
            }
        }

        return result::Success;
    }

    result Instance::impl_createDevice(const device_desc& desc, Device** device)
    {
        auto* output = new Device();
        output->m_desc = desc;
        output->m_adapter = desc.adapter;
        output->m_validationCallbackMessenger = m_validationCallbackMessenger;

        // Queue creation
        auto families = detail::findQueueFamilies(static_cast<VkPhysicalDevice>(desc.adapter->m_ptr));

        std::vector<float> graphicsPriorities;
        std::vector<float> computePriorities;
        std::vector<float> transferPriorities;

        for (size_t i = 0; i < desc.numQueues; i++)
        {
            auto& queueDesc = desc.queues[i];

            float priority = 0;
            switch (queueDesc.priority)
            {
                case queue_priority::Normal:
                    priority = 0.5f;
                    break;
                case queue_priority::High:
                    priority = 1.0f;
                    break;
            }

            switch (queueDesc.type)
            {
                case queue_type::Graphics:
                    graphicsPriorities.push_back(priority);
                    break;
                case queue_type::Compute:
                    computePriorities.push_back(priority);
                    break;
                case queue_type::Transfer:
                    transferPriorities.push_back(priority);
                    break;
            }
        }

        std::vector<VkDeviceQueueCreateInfo> queues;

        if (!graphicsPriorities.empty())
            queues.push_back(VkDeviceQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, {},
                families[queue_type::Graphics],
                static_cast<uint32_t>(graphicsPriorities.size()), graphicsPriorities.data() });

        if (!computePriorities.empty())
            queues.push_back(VkDeviceQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, {},
                families[queue_type::Compute],
                static_cast<uint32_t>(computePriorities.size()), computePriorities.data() });

        if (!transferPriorities.empty())
            queues.push_back(VkDeviceQueueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, {},
                families[queue_type::Transfer],
                static_cast<uint32_t>(transferPriorities.size()), transferPriorities.data() });

        // Extensions
        std::vector<const char*> extensions;

        if (desc.adapter->m_nodeCount > 1)
            extensions.push_back("VK_KHR_device_group");
        
#ifdef __APPLE__
        // required for MoltenVK
        extensions.push_back("VK_KHR_portability_subset");
#endif

        // Features
        VkPhysicalDeviceFeatures features{};

        // Create device
        VkDeviceCreateInfo ci{
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            nullptr,
            {},
            static_cast<uint32_t>(queues.size()), queues.data(),
            0, nullptr, // Vulkan device layers are deprecated
            static_cast<uint32_t>(extensions.size()), extensions.data(),
            &features
        };

        VkDevice vkDevice = nullptr;
        const VkResult r = vkCreateDevice(static_cast<VkPhysicalDevice>(desc.adapter->m_ptr), &ci, nullptr, &vkDevice);
        if (r != VK_SUCCESS)
        {
            destroyDevice(output);
            return detail::mapVkResult(r);
        }
        output->m_ptr = vkDevice;

        // Load function table
        auto* table = new VolkDeviceTable();
        volkLoadDeviceTable(table, vkDevice);
        output->m_functionTable = table;

        // Get created queues
        std::unordered_map<queue_type, uint32_t> queueCounts {
            { queue_type::Graphics, 0 },
            { queue_type::Compute, 0 },
            { queue_type::Transfer, 0 }
        };

        for (size_t i = 0; i < desc.numQueues; i++)
        {
            auto& queueDesc = desc.queues[i];

            VkQueue vkQueue;
            table->vkGetDeviceQueue(vkDevice, families[queueDesc.type], queueCounts[queueDesc.type], &vkQueue);

            auto* queue = new Queue();
            queue->m_desc = queueDesc;
            queue->m_device = output;
            queue->m_ptrs = std::vector<Queue::native_queue*>(desc.adapter->m_nodeCount, vkQueue);
            queue->m_validationCallbackMessenger = output->m_validationCallbackMessenger;

            switch(queueDesc.type)
            {
                case queue_type::Graphics:
                    output->m_graphicsQueues.push_back(queue);
                    break;
                case queue_type::Compute:
                    output->m_computeQueues.push_back(queue);
                    break;
                case queue_type::Transfer:
                    output->m_transferQueues.push_back(queue);
                    break;
            }

            queueCounts[queueDesc.type]++;
        }
        
        // create work resources
        if (queueCounts[queue_type::Graphics] > 0)
            output->m_workQueueType = queue_type::Graphics;
        else if (queueCounts[queue_type::Compute] > 0)
            output->m_workQueueType = queue_type::Compute;
        else
            output->m_workQueueType = queue_type::Transfer;
        
        VkCommandPoolCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = {};
        createInfo.queueFamilyIndex = families[output->m_workQueueType];
        table->vkCreateCommandPool(vkDevice, &createInfo, nullptr, reinterpret_cast<VkCommandPool*>(&output->m_workCmdGroup));
        
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        allocInfo.commandPool = static_cast<VkCommandPool>(output->m_workCmdGroup);
        table->vkAllocateCommandBuffers(vkDevice, &allocInfo, reinterpret_cast<VkCommandBuffer*>(&output->m_workCmdList));
        
        VkFenceCreateInfo fenceInfo {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = nullptr;
        fenceInfo.flags = {};
        table->vkCreateFence(vkDevice, &fenceInfo, nullptr, reinterpret_cast<VkFence*>(&output->m_workFence));
        
        *device = output;
        return result::Success;
    }

    void Instance::impl_destroyDevice(Device* device)
    {
        // Cleanup queue wrappers
        for (auto* graphics : device->m_graphicsQueues)
            delete graphics;

        for (auto* compute : device->m_computeQueues)
            delete compute;
        
        for (auto* transfer : device->m_transferQueues)
            delete transfer;
        
        // Cleanup work objects
        if (device->m_workFence)
            static_cast<VolkDeviceTable*>(device->m_functionTable)->vkDestroyFence(static_cast<VkDevice>(device->m_ptr), static_cast<VkFence>(device->m_workFence), nullptr);
        if (device->m_workCmdGroup)
            static_cast<VolkDeviceTable*>(device->m_functionTable)->vkDestroyCommandPool(static_cast<VkDevice>(device->m_ptr), static_cast<VkCommandPool>(device->m_workCmdGroup), nullptr);

        // Delete device
        if (device->m_ptr)
            static_cast<VolkDeviceTable*>(device->m_functionTable)->vkDestroyDevice(static_cast<VkDevice>(device->m_ptr), nullptr);

        // Delete function table
        delete static_cast<VolkDeviceTable*>(device->m_functionTable);

        // Delete device wrapper
        delete device;
    }

    result Instance::impl_createSurfaceEXT([[maybe_unused]] const surface_win32_desc_ext& desc, [[maybe_unused]] SurfaceEXT** surface)
    {
#ifndef VK_USE_PLATFORM_WIN32_KHR
        return result::ErrorExtensionNotSupported;
#else
        VkWin32SurfaceCreateInfoKHR info {};
        info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        info.pNext = nullptr;
        info.flags = {};
        info.hinstance = static_cast<HINSTANCE>(desc.hinstance);
        info.hwnd = static_cast<HWND>(desc.hwnd);

        VkSurfaceKHR vkSurface;
        const auto r = vkCreateWin32SurfaceKHR(static_cast<VkInstance>(m_ptr), &info, nullptr, &vkSurface);
        if (r != VK_SUCCESS)
            return detail::mapVkResult(r);

        auto* output = new SurfaceEXT();
        output->m_ptr = vkSurface;

        *surface = output;
        return result::Success;
#endif
    }

    result Instance::impl_createSurfaceEXT([[maybe_unused]] const surface_cocoa_desc_ext& desc, [[maybe_unused]] SurfaceEXT** surface)
    {
#ifndef VK_USE_PLATFORM_METAL_EXT
        return result::ErrorExtensionNotSupported;
#else
        
        VkMetalSurfaceCreateInfoEXT info {};
        info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        info.pNext = nullptr;
        info.flags = {};
        info.pLayer = detail::getCAMetalLayer(desc.nsWindow);
        
        VkSurfaceKHR vkSurface;
        const auto r = vkCreateMetalSurfaceEXT(static_cast<VkInstance>(m_ptr), &info, nullptr, &vkSurface);
        if (r != VK_SUCCESS)
            return detail::mapVkResult(r);
        
        auto* output = new SurfaceEXT();
        output->m_ptr = vkSurface;
        
        *surface = output;
        return result::Success;
#endif
    }

    result Instance::impl_createSurfaceEXT([[maybe_unused]] const surface_xlib_desc_ext& desc, [[maybe_unused]] SurfaceEXT** surface)
    {
#ifndef VK_USE_PLATFORM_XLIB_KHR
        return result::ErrorExtensionNotSupported;
#else
        VkXlibSurfaceCreateInfoKHR info {};
        info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        info.pNext = nullptr;
        info.flags = {};
        info.dpy = static_cast<_XDisplay*>(desc.display);
        info.window = desc.window;

        VkSurfaceKHR vkSurface;
        const auto r = vkCreateXlibSurfaceKHR(static_cast<VkInstance>(m_ptr), &info, nullptr, &vkSurface);
        if (r != VK_SUCCESS)
            return detail::mapVkResult(r);

        auto* output = new SurfaceEXT();
        output->m_ptr = vkSurface;

        *surface = output;
        return result::Success;
#endif
    }

    result Instance::impl_createSurfaceEXT([[maybe_unused]] const surface_xcb_desc_ext& desc, [[maybe_unused]] SurfaceEXT** surface)
    {
#ifndef VK_USE_PLATFORM_XCB_KHR
        return result::ErrorExtensionNotSupported;
#else
        VkXcbSurfaceCreateInfoKHR info {};
        info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        info.pNext = nullptr;
        info.flags = {};
        info.connection = static_cast<xcb_connection_t*>(desc.connection);
        info.window = desc.window;

        VkSurfaceKHR vkSurface;
        const auto r = vkCreateXcbSurfaceKHR(static_cast<VkInstance>(m_ptr), &info, nullptr, &vkSurface);
        if (r != VK_SUCCESS)
            return detail::mapVkResult(r);

        auto* output = new SurfaceEXT();
        output->m_ptr = vkSurface;

        *surface = output;
        return result::Success;
#endif
    }

    void Instance::impl_destroySurfaceEXT(SurfaceEXT* surface)
    {
        vkDestroySurfaceKHR(static_cast<VkInstance>(m_ptr), static_cast<VkSurfaceKHR>(surface->m_ptr), nullptr);
        delete surface;
    }
}
