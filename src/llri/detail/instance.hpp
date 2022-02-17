/**
 * @file instance.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    enum struct result : uint8_t;
    enum struct instance_extension : uint8_t;
    class Instance;
    class Adapter;

    struct device_desc;
    class Device;

    struct surface_win32_desc_ext;
    struct surface_cocoa_desc_ext;
    struct surface_xlib_desc_ext;
    struct surface_xcb_desc_ext;
    class SurfaceEXT;

    /**
     * @brief Instance description to be used in llri::createInstance().
    */
    struct instance_desc
    {
        /**
         * @brief The number of instance extensions in the instance_desc::extensions array.
         *
         * @note Valid usage (ErrorExceededLimit): As instance_desc::extensions can only hold unique values, numExtensions can not be more than instance_extension::MaxEnum + 1.
        */
        uint32_t numExtensions;
        /**
         * @brief An array of instance extensions, [extensions, extensions + numExtensions - 1].
         *
         * @note Valid usage (ErrorInvalidUsage): If instance_desc::numExtensions > 0, then this **must** be a valid pointer to an array of instance_extension. If instance_desc::numExtensions == 0, then this pointer **may** be nullptr.
         * @note Valid usage (ErrorInvalidUsage): Each value in this array **must** be <= instance_extension::MaxEnum.
         * @note Valid usage (ErrorInvalidUsage): Each value in this array **must** be unique.
         * @note Valid usage (ErrorExtensionNotSupported): queryInstanceExtensionSupport() must return true on each value in this array.
        */
        instance_extension* extensions;
        /**
         * @brief Sets the name of the application in the implementation if applicable.
         *
         * @note This parameter is not guaranteed to be used but is known to at least apply to Vulkan.
         * @note Valid usage: Can be nullptr or a valid null-terminated string.
        */
        const char* applicationName;
    };

    namespace detail
    {
        result impl_createInstance(const instance_desc& desc, Instance** instance, bool enableImplementationMessagePolling);
        void impl_destroyInstance(Instance* instance);

        using messenger_type = void;
        /**
         * @brief Polls API messages, only called if LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING is not defined.
         * Used internally only
         * @param messenger This value may differ depending on the function that is calling it, the most relevant messenger will be picked.
        */
        void impl_pollAPIMessages(messenger_type* messenger);
    }

    /**
     * @brief Create an llri Instance, which can be used to enumerate adapters and create a few core objects.
     * Like with all API objects, the user is responsible for destroying the Instance again using destroyInstance().
     *
     * @param desc A structure describing how the instance should be created.
     * @param instance A pointer to an Instance* variable. Used to write the resulting instance to.
     *
     * @note Valid usage (ErrorInvalidUsage): instance **must**  be a valid non-null pointer.
     * @note Valid usage (ErrorExceededLimit): Only one Instance object may exist at a time. // TODO: Enforce in validation
     *
     * @return Success upon correct execution of the operation.
     * @return instance_desc: ErrorInvalidUsage, ErrorExceededLimit, ErrorExtensionNotSupported.
     * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorInitializationFailed, ErrorIncompatibleDriver.
    */
    result createInstance(const instance_desc& desc, Instance** instance);

    /**
     * @brief Destroys the given instance and its directly related internal resources.
     * All resources created through the instance **must** be destroyed prior to calling this function.
     *
     * @param instance The instance object to destroy.
     *
     * @note Valid usage: instance **must** be a valid instance pointer or nullptr.
    */
    void destroyInstance(Instance* instance);

    /**
     * @brief Instance is the central object of the application and is used to create most other API objects.
     * Only a single object of Instance **may** exist within an application. Creating more instances is not supported.
    */
    class Instance
    {
        friend result detail::impl_createInstance(const instance_desc& desc, Instance** instance, bool enableImplementationMessagePolling);
        friend void detail::impl_destroyInstance(Instance* instance);

        friend result llri::createInstance(const instance_desc& desc, Instance** instance);
        friend void llri::destroyInstance(Instance* instance);

        friend class Adapter;
        friend class Device;

    public:
        using native_instance = void;
        
        /**
         * @brief Get the desc that the Instance was created with.
         */
        [[nodiscard]] instance_desc getDesc() const;
        
        /**
         * @brief Gets the native Instance pointer, which depending on the llri::getImplementation() is a pointer to the following:
         *
         * DirectX12: IDXGIFactory*
         * Vulkan: VkInstance
         */
        [[nodiscard]] native_instance* getNative() const;

        /**
         * @brief Retrieve a vector of adapters available to this application. Adapters usually represent PCIe devices such as GPUs.
         *
         * Using this function, you can select one or more adapters for Device creation.
         * The adapters returned by this function **may** represent one or more physical adapters depending on system hardware configuration. Hardware features like SLI/Crossfire cause adapters to be linked together and be listed as a single Adapter.
         *
         * @param adapters The vector to fill with available adapters. The vector is cleared at the start of the operation and is only filled if the operation succeeds.
         *
         * @note Valid usage (ErrorInvalidUsage): adapters **must** be a valid pointer to a std::vector<Adapter*> variable.
         *
         * @return Success upon correct execution of the operation.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorInitializationFailed.
        */
        result enumerateAdapters(std::vector<Adapter*>* adapters);

        /**
         * @brief Creates a virtual LLRI device. Device represents one or multiple adapters and is used to allocate memory, create resources, or send commands to the adapter.
         *
         * If the adapter that the Device is created for contains multiple adapter nodes, then the device will also represent the said adapter nodes.
         *
         * @param desc The description of the device.
         * @param device A pointer to a device pointer variable. The pointer variable will be set to the resulting device upon successful execution.
         *
         * @note Valid usage (ErrorInvalidUsage): device **must** be a valid pointer to a Device* variable.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorDeviceLost if the adapter was lost.
         * @return device_desc defined result values: ErrorInvalidUsage, ErrorFeatureNotSupported, ErrorExceededLimit, ErrorExtensionNotSupported
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result createDevice(const device_desc& desc, Device** device);

        /**
         * @brief Destroy the device object.
         * All resources created through the device **must** be destroyed prior to calling this function.
         *
         * @param device The device object to be destroyed.
         *
         * @note Valid usage: device **must** be a valid device pointer or nullptr.
        */
        void destroyDevice(Device* device);

        /**
         * @brief Create a SurfaceEXT object for a Win32 HWND window.
         * @param desc A description of how the surface should be created.
         * @param surface A pointer to the resulting surface variable.
         *
         * @note Valid usage (ErrorInvalidUsage): surface **must** be a valid non-null pointer to a SurfaceEXT* variable.
         * @note Valid usage (ErrorExtensionNotEnabled): instance_extension::SurfaceWin32 **must** be enabled in the instance.
         *
         * @return Success upon correct execution of the operation.
         * @return surface_win32_desc_ext defined result values: ErrorInvalidUsage.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory.
        */
        result createSurfaceEXT(const surface_win32_desc_ext& desc, SurfaceEXT** surface);

        /**
         * @brief Create a SurfaceEXT object for a Cocoa window, using CAMetalLayer.
         * @param desc A description of how the surface should be created.
         * @param surface A pointer to the resulting surface variable.
         *
         * @note Valid usage (ErrorInvalidUsage): surface **must** be a valid non-null pointer to a SurfaceEXT* variable.
         * @note Valid usage (ErrorExtensionNotEnabled): instance_extension::SurfaceCocoa **must** be enabled in the instance.
         *
         * @return Success upon correct execution of the operation.
         * @return surface_cocoa_desc_ext defined result values: ErrorInvalidUsage.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory
        */
        result createSurfaceEXT(const surface_cocoa_desc_ext& desc, SurfaceEXT** surface);

        /**
         * @brief Create a SurfaceEXT object for an Xlib window.
         * @param desc A description of how the surface should be created.
         * @param surface A pointer to the resulting surface variable.
         *
         * @note Valid usage (ErrorInvalidUsage): surface **must** be a valid non-null pointer to a SurfaceEXT* variable.
         * @note Valid usage (ErrorExtensionNotEnabled): instance_extension::SurfaceXlib **must** be enabled in the instance.
         *
         * @return Success upon correct execution of the operation.
         * @return surface_xlib_desc_ext defined result values: ErrorInvalidUsage.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory
         */
        result createSurfaceEXT(const surface_xlib_desc_ext& desc, SurfaceEXT** surface);

        /**
         * @brief Create a SurfaceEXT object for an XCB window.
         * @param desc A description of how the surface should be created.
         * @param surface A pointer to the resulting surface variable.
         *
         * @note Valid usage (ErrorInvalidUsage): surface **must** be a valid non-null pointer to a SurfaceEXT* variable.
         * @note Valid usage (ErrorExtensionNotEnabled): instance_extension::SurfaceXcb **must** be enabled in the instance.
         *
         * @return Success upon correct execution of the operation.
         * @return surface_xcb_desc_ext defined result values: ErrorInvalidUsage.
         * @return Implementation defined result values: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory
         */
        result createSurfaceEXT(const surface_xcb_desc_ext& desc, SurfaceEXT** surface);

        /**
         * @brief Destroy the SurfaceEXT object.
         * @param surface The SurfaceEXT to destroy, **must** be a valid SurfaceEXT pointer or nullptr.
        */
        void destroySurfaceEXT(SurfaceEXT* surface);

    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Instance() = default;
        ~Instance() = default;

        instance_desc m_desc;

        native_instance* m_ptr = nullptr;

        bool m_shouldConstructValidationCallbackMessenger;
        detail::messenger_type* m_validationCallbackMessenger = nullptr; // Allows API to store their callback messenger if needed

        std::unordered_map<void*, Adapter*> m_cachedAdapters;

#ifdef LLRI_DETAIL_ENABLE_VALIDATION
        std::unordered_set<instance_extension> m_enabledExtensions;
#endif

        result impl_enumerateAdapters(std::vector<Adapter*>* adapters);
        result impl_createDevice(const device_desc& desc, Device** device);
        void impl_destroyDevice(Device* device);

        result impl_createSurfaceEXT(const surface_win32_desc_ext& desc, SurfaceEXT** surface);
        result impl_createSurfaceEXT(const surface_cocoa_desc_ext& desc, SurfaceEXT** surface);
        result impl_createSurfaceEXT(const surface_xlib_desc_ext& desc, SurfaceEXT** surface);
        result impl_createSurfaceEXT(const surface_xcb_desc_ext& desc, SurfaceEXT** surface);
        void impl_destroySurfaceEXT(SurfaceEXT* surface);
    };
}
