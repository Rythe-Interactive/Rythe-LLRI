/**
 * @file instance.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    inline std::string to_string(instance_extension ext)
    {
        switch (ext)
        {
            case instance_extension::DriverValidation:
                return "DriverValidation";
            case instance_extension::GPUValidation:
                return "GPUValidation";
            case instance_extension::SurfaceWin32:
                return "SurfaceWin32";
            case instance_extension::SurfaceCocoa:
                return "SurfaceCocoa";
            case instance_extension::SurfaceXlib:
                return "SurfaceXlib";
            case instance_extension::SurfaceXcb:
                return "SurfaceXcb";
        }

        return "Invalid instance_extension value";
    }

    [[nodiscard]] inline bool queryInstanceExtensionSupport(instance_extension ext)
    {
        return detail::queryInstanceExtensionSupport(ext);
    }

    inline result createInstance(const instance_desc& desc, Instance** instance)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(instance != nullptr, result::ErrorInvalidUsage)
        *instance = nullptr;

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.numExtensions <= static_cast<uint32_t>(instance_extension::MaxEnum) + 1, result::ErrorExceededLimit)
        
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.numExtensions == 0 || (desc.numExtensions > 0 && desc.extensions != nullptr), result::ErrorInvalidUsage)

#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        for (size_t i = 0; i < desc.numExtensions; i++)
        {
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(desc.extensions[i] <= instance_extension::MaxEnum, i, result::ErrorExtensionNotSupported)
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(queryInstanceExtensionSupport(desc.extensions[i]), i, result::ErrorExtensionNotSupported)
        }
#endif

        // handle validation parameter
#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        constexpr bool enableValidation = true;
#else
        constexpr bool enableValidation = false;
#endif

        // create instance
        const auto r = detail::impl_createInstance(desc, instance, enableValidation);

        // handle message polling
#ifndef LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
        if (*instance)
            detail::impl_pollAPIMessages((*instance)->m_validationCallbackMessenger);
#endif

        // handle validation data
#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        if (*instance)
            (*instance)->m_enabledExtensions = { desc.extensions, desc.extensions + desc.numExtensions };
#endif

        // finally return
        return r;
    }

    inline void destroyInstance(Instance* instance)
    {
        if (!instance)
            return;

        detail::impl_destroyInstance(instance);
        // Can't do any polling after the instance is destroyed
    }

    inline instance_desc Instance::getDesc() const
    {
        return m_desc;
    }

    inline Instance::native_instance* Instance::getNative() const
    {
        return m_ptr;
    }

    inline result Instance::enumerateAdapters(std::vector<Adapter*>* adapters)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(adapters != nullptr, result::ErrorInvalidUsage)

        adapters->clear();

        // Clear internal pointers, lost adapters will have a nullptr m_ptr
        for (auto& [ptr, adapter] : m_cachedAdapters)
            adapter->m_ptr = nullptr;

        LLRI_DETAIL_CALL_IMPL(impl_enumerateAdapters(adapters), m_validationCallbackMessenger)
    }

    inline result Instance::createDevice(const device_desc& desc, Device** device)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(device != nullptr, result::ErrorInvalidUsage)

        *device = nullptr;

#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.adapter != nullptr, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.numExtensions <= static_cast<uint32_t>(adapter_extension::MaxEnum) + 1, result::ErrorInvalidUsage);
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.numExtensions == 0 || (desc.numExtensions > 0 && desc.extensions != nullptr), result::ErrorInvalidUsage)

        for (size_t i = 0; i < desc.numExtensions; i++)
        {
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(desc.extensions[i] <= adapter_extension::MaxEnum, i, result::ErrorExtensionNotSupported)
            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(desc.adapter->queryExtensionSupport(desc.extensions[i]), i, result::ErrorExtensionNotSupported)
        }

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.adapter->m_ptr != nullptr, result::ErrorDeviceLost)

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.numQueues != 0, result::ErrorInvalidUsage);
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.queues != nullptr, result::ErrorInvalidUsage);

        // Get max queues
        std::unordered_map<queue_type, uint8_t> maxQueueCounts {
            { queue_type::Graphics, desc.adapter->queryQueueCount(queue_type::Graphics) },
            { queue_type::Compute, desc.adapter->queryQueueCount(queue_type::Compute) },
            { queue_type::Transfer, desc.adapter->queryQueueCount(queue_type::Transfer) }
        };
        
        // Validate all queue descs and their relation to max queue counts
        std::unordered_map<queue_type, size_t> queueCounts {
            { queue_type::Graphics, 0 },
            { queue_type::Compute, 0 },
            { queue_type::Transfer, 0 }
        };

        for (size_t i = 0; i < desc.numQueues; i++)
        {
            auto& queue = desc.queues[i];

            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(desc.queues[i].type <= queue_type::MaxEnum, i, result::ErrorInvalidUsage)

            queueCounts[queue.type]++; // count the number of queues of this given type to notexceed the max
            LLRI_DETAIL_VALIDATION_REQUIRE_MESSAGE(queueCounts[queue.type] <= maxQueueCounts[queue.type], "queue_desc " + std::to_string(i) + " is the " + std::to_string(queueCounts[queue.type]) + "th " +
                    to_string(queue.type) + " queue, even though the maximum number of queues of this type is " + std::to_string(maxQueueCounts[queue.type]) + ".", result::ErrorInvalidUsage)

            LLRI_DETAIL_VALIDATION_REQUIRE_ITER(queue.priority <= queue_priority::MaxEnum, i, result::ErrorInvalidUsage)
        }
#endif

        LLRI_DETAIL_CALL_IMPL(impl_createDevice(desc, device), (*device)->m_validationCallbackMessenger)
    }

    inline void Instance::destroyDevice(Device* device)
    {
        if (!device)
            return;

        impl_destroyDevice(device);

        LLRI_DETAIL_POLL_API_MESSAGES(m_validationCallbackMessenger)
    }

    inline result Instance::createSurfaceEXT(const surface_win32_desc_ext& desc, SurfaceEXT** surface)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(surface != nullptr, result::ErrorInvalidUsage)
        *surface = nullptr;
        LLRI_DETAIL_VALIDATION_REQUIRE(m_enabledExtensions.find(instance_extension::SurfaceWin32) != m_enabledExtensions.end(), result::ErrorExtensionNotEnabled)

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.hinstance != nullptr, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.hwnd != nullptr, result::ErrorInvalidUsage)

        LLRI_DETAIL_CALL_IMPL(impl_createSurfaceEXT(desc, surface), m_validationCallbackMessenger)
    }
    
    inline result Instance::createSurfaceEXT(const surface_cocoa_desc_ext& desc, SurfaceEXT** surface)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(surface != nullptr, result::ErrorInvalidUsage)
        *surface = nullptr;
        LLRI_DETAIL_VALIDATION_REQUIRE(m_enabledExtensions.find(instance_extension::SurfaceCocoa) != m_enabledExtensions.end(), result::ErrorExtensionNotEnabled)

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.nsWindow != nullptr, result::ErrorInvalidUsage)
        
        LLRI_DETAIL_CALL_IMPL(impl_createSurfaceEXT(desc, surface), m_validationCallbackMessenger)
    }

    inline result Instance::createSurfaceEXT(const surface_xlib_desc_ext& desc, SurfaceEXT** surface)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(surface != nullptr, result::ErrorInvalidUsage)
        *surface = nullptr;
        LLRI_DETAIL_VALIDATION_REQUIRE(m_enabledExtensions.find(instance_extension::SurfaceXlib) != m_enabledExtensions.end(), result::ErrorExtensionNotEnabled)

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.display != nullptr, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.window != 0, result::ErrorInvalidUsage)

        LLRI_DETAIL_CALL_IMPL(impl_createSurfaceEXT(desc, surface), m_validationCallbackMessenger)
    }

    inline result Instance::createSurfaceEXT(const surface_xcb_desc_ext& desc, SurfaceEXT** surface)
    {
        LLRI_DETAIL_VALIDATION_REQUIRE(surface != nullptr, result::ErrorInvalidUsage)
        *surface = nullptr;
        LLRI_DETAIL_VALIDATION_REQUIRE(m_enabledExtensions.find(instance_extension::SurfaceXcb) != m_enabledExtensions.end(), result::ErrorExtensionNotEnabled)

        LLRI_DETAIL_VALIDATION_REQUIRE(desc.connection != nullptr, result::ErrorInvalidUsage)
        LLRI_DETAIL_VALIDATION_REQUIRE(desc.window != 0, result::ErrorInvalidUsage)

        LLRI_DETAIL_CALL_IMPL(impl_createSurfaceEXT(desc, surface), m_validationCallbackMessenger)
    }

    inline void Instance::destroySurfaceEXT(SurfaceEXT* surface)
    {
        if (!surface)
            return;

        impl_destroySurfaceEXT(surface);
        LLRI_DETAIL_POLL_API_MESSAGES(m_validationCallbackMessenger)
    }
}
