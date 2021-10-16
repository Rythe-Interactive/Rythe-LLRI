/**
 * @file instance.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    enum struct result : uint8_t;
    struct instance_extension;
    class Instance;
    class Adapter;

    struct device_desc;
    class Device;

    /**
     * @brief Instance description to be used in llri::createInstance().
    */
    struct instance_desc
    {
        /**
         * @brief The number of instance extensions in the instance_desc::extensions array.
        */
        uint32_t numExtensions;
        /**
         * @brief The instance extensions, if instance_desc::numExtensions > 0, then this **must** be a valid pointer to an array of instance_extension.
         * If instance_desc::numExtensions == 0, then this pointer **may** be nullptr.
        */
        instance_extension* extensions;
        /**
         * @brief Sets the name of the application in the implementation if applicable.
         * @note This parameter is not guaranteed to be used but is known to at least apply to Vulkan.
        */
        const char* applicationName;
    };

    /**
     * @brief Internal functions, don't use outside of the API.
    */
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
     * @brief Create an llri Instance.
     * Like with all API objects, the user is responsible for destroying the Instance again using destroyInstance().
     * @param desc The description of the instance.
     * @param instance Must be a valid pointer to an Instance pointer variable. Upon successful execution of the operation the pointer is set to the resulting instance object.
     *
     * @return Success upon correct execution of the operation.
     * @return ErrorInvalidUsage if the instance is nullptr
     * @return ErrorInvalidUsage if desc.numExtensions > 0 and desc.extensions is nullptr.
     * @return ErrorExtensionNotSupported if any of the extensions fail to be created.
     * @return Implementation defined result values: ErrorExtensionNotSupported, ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorInitializationFailed, ErrorIncompatibleDriver.
    */
    result createInstance(const instance_desc& desc, Instance** instance);

    /**
     * @brief Destroys the given instance and its directly related internal resources.
     * All resources created through the instance **must** be destroyed prior to calling this function.
     *
     * @param instance The instance to destroy. This value **must** be a valid instance pointer or nullptr.
    */
    void destroyInstance(Instance* instance);

    /**
     * @brief Instance is the central resource of the application and is used to create most other API objects.
     * Only a single instance of Instance **may** exist within an application. Creating more instances is not officially supported.
    */
    class Instance
    {
        friend result detail::impl_createInstance(const instance_desc& desc, Instance** instance, bool enableImplementationMessagePolling);
        friend void detail::impl_destroyInstance(Instance* instance);

        friend result llri::createInstance(const instance_desc& desc, Instance** instance);
        friend void llri::destroyInstance(Instance* instance);

    public:
        /**
         * @brief Retrieve a vector of adapters available to this application. Adapters usually represent PCIe devices such as GPUs.
         *
         * Using this function, you can select one or more adapters for Device creation.
         * The adapters returned by this function **may** represent one or more physical adapters depending on system hardware configuration. Hardware features like SLI/Crossfire cause adapters to be linked together and be listed as a single Adapter.
         *
         * @param adapters The vector to fill with available adapters. The vector is cleared at the start of the operation and is only filled if the operation succeeds.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if adapters is nullptr.
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
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if device is nullptr.
         * @return ErrorInvalidUsage if desc.adapter is nullptr.
         * @return ErrorInvalidUsage if desc.numExtensions is more than 0 and desc.extensions is nullptr.
         * @return ErrorInvalidUsage if desc.numQueues is less than 1.
         * @return ErrorInvalidUsage if desc.queues is nullptr.
         * @return ErrorInvalidUsage if more queues of a given type are requested than the maximum number of queues for that given type.
         * @return ErrorInvalidUsage if any of the queue_desc's types was an invalid queue_type value.
         * @return ErrorInvalidUsage if any of the queue_desc's priorities was an invalid queue_priority value.
         * @return ErrorDeviceLost if the adapter was lost.
        */
        result createDevice(const device_desc& desc, Device** device);

        /**
         * @brief Destroy the LLRI device.
         *
         * All resources created through the device **must** be destroyed prior to calling this function.
         *
         * @param device The device, **must** be a valid device pointer or nullptr.
        */
        void destroyDevice(Device* device);

    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Instance() = default;
        ~Instance() = default;

        void* m_ptr = nullptr;
        void* m_debugAPI = nullptr;
        void* m_debugGPU = nullptr;

        bool m_shouldConstructValidationCallbackMessenger;
        void* m_validationCallbackMessenger = nullptr; //Allows API to store their callback messenger if needed

        std::unordered_map<void*, Adapter*> m_cachedAdapters;

        result impl_enumerateAdapters(std::vector<Adapter*>* adapters);
        result impl_createDevice(const device_desc& desc, Device** device);
        void impl_destroyDevice(Device* device);
    };
}
