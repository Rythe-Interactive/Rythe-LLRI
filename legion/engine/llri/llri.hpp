#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <llri/instance_extensions.hpp>
#include <llri/adapter_extensions.hpp>

namespace legion::graphics::llri
{
    class InstanceT;
    /**
     * @brief Handle to the core llri::Instance. Usually only a single instance of Instance exists within an application, but if so desired, multiple Instance instances are supported.
     * Instance is the center of the application and is used to create most other API objects.
    */
    using Instance = InstanceT*;

    class AdapterT;
    /**
     * @brief Handle to a compatible adapter (GPU, APU, IGPU, etc.).
     * This handle is created and owned by the Instance, the user is not responsible for destroying it.
    */
    using Adapter = AdapterT*;

    class DeviceT;
    /**
     * @brief Handle to a virtual LLRI Device.
     * A device is a virtual representation of an adapter and can create/destroy resources for the said adapter.
     */
    using Device = DeviceT*;
    struct device_desc;

    /**
     * @brief Informative result values for llri operations.
     *
     * Operations that execute properly will return result::Success, or if they fail they will pick the appropriate failing result value. Note that some result values are prefixed with "Error", implying that their result value was fatal and can not be recovered from. Failures without the "Error" prefix are often soft failures that might for example be caused by user-defined timeouts.
    */
    enum struct result
    {
        /**
         * @brief The function executed properly.
        */
        Success = 0,
        /**
         * @brief The function's execution time exceeded a user-defined timeout.
        */
        Timeout,
        /**
         * @brief A fence or query has not yet completed.
        */
        NotReady,
        /**
         * @brief This error is caused by improper error mapping by the LLRI implementation, and should under normal circumstances never occur.
         * If this value is returned, it is likely caused by a bug in the API. Consider contacting the authors for more information.
        */
        ErrorUnknown,
        /**
         * @brief The usage of the operation was invalid.
         * This is usually due to incorrect API usage.
        */
        ErrorInvalidUsage,
        /**
         * @brief The requested feature is not supported.
        */
        ErrorFeatureNotSupported,
        /**
         * @brief The requested extension is not supported.
        */
        ErrorExtensionNotSupported,
        /**
         * @brief The operation caused the device to become non-responsive for a long period of time. This is often caused by badly formed commands sent by the application. 
        */
        ErrorDeviceHung,
        /**
         * @brief A device may be lost after invalid API usage causes fatal errors that the device can not recover from.
         * The device immediately becomes invalid and must be destroyed and recreated.
         *
         * After a device loss, the application must reiterate over available Adapters as the previously used Adapter may have become invalid.
        */
        ErrorDeviceLost,
        /**
         * @brief The video card has been physically removed from the system. The device immediately becomes invalid and must be destroyed and recreated. For this, the application must reiterate over available Adapters as the previously used Adapter has become invalid.
        */
        ErrorDeviceRemoved,
        /**
         * @brief An internal driver error was thrown. After this, the device will be put into the device lost state and will become invalid. See ErrorDeviceLost for more information on device loss.
        */
        ErrorDriverFailure,
        /**
         * @brief A CPU side allocation failed because the system ran out of memory.
        */
        ErrorOutOfHostMemory,
        /**
         * @brief A GPU side allocation failed because the GPU ran out of memory.
        */
        ErrorOutOfDeviceMemory,
        /**
         * @brief Initialization of an object failed because of internal or implementation specific reasons.
        */
        ErrorInitializationFailed,
        /**
         * @brief The requested internal API version (DirectX, Vulkan, etc.) is not supported by the driver.
        */
        ErrorIncompatibleDriver
    };

    /**
     * @brief Converts a result to a string to aid in debug logging.
    */
    constexpr const char* to_string(const result& result)
    {
        switch (result)
        {
            case result::Success:
                return "Success";
            case result::Timeout:
                return "Timeout";
            case result::ErrorUnknown:
                return "ErrorUnknown";
            case result::ErrorInvalidUsage:
                return "ErrorInvalidUsage";
            case result::ErrorFeatureNotSupported:
                return "ErrorFeatureNotSupported";
            case result::ErrorExtensionNotSupported:
                return "ErrorExtensionNotSupported";
            case result::ErrorDeviceHung:
                return "ErrorDeviceHung";
            case result::ErrorDeviceLost:
                return "ErrorDeviceLost";
            case result::ErrorDeviceRemoved:
                return "ErrorDeviceRemoved";
            case result::ErrorDriverFailure:
                return "ErrorDriverFailure";
            case result::NotReady:
                return "NotReady";
            case result::ErrorOutOfHostMemory:
                return "ErrorOutOfHostMemory";
            case result::ErrorOutOfDeviceMemory:
                return "ErrorOutOfDeviceMemory";
            case result::ErrorInitializationFailed:
                return "ErrorInitializationFailed";
            case result::ErrorIncompatibleDriver:
                return "ErrorIncompatibleDriver";
        }

        return "Invalid result value";
    }

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
         * @brief The instance extensions, if instance_desc::numExtensions > 0, then this has to be a valid pointer to an array of instance_extension.
         * If numExtensions == 0, then this pointer may be nullptr.
        */
        instance_extension* extensions;
        /**
         * @brief Sets the name of the application in internal API if applicable.
         * This is not guaranteed but is known to at least apply to Vulkan.
        */
        const char* applicationName;
    };

    /**
     * @brief Create an llri Instance.
     * Like with all API objects, the user is responsible for destroying the instance again using destroyInstance().
     * @param desc The description of the instance.
     * @param instance Must be a valid pointer to an Instance variable.
     *
     * @return Success upon correct execution of the operation.
     * @return ErrorInvalidUsage if the instance is nullptr, or if desc.numExtensions > 0 and desc.extensions is nullptr.
     * @return ErrorExtensionNotSupported if any of the extensions fail to be created.
     * @return Otherwise it may also return: ErrorExtensionNotSupported, ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorInitializationFailed, ErrorIncompatibleDriver.
    */
    result createInstance(const instance_desc& desc, Instance* instance);

    /**
     * @brief Destroys the given instance and its directly related internal resources (debug info etc).
     * All resources created through the instance must be destroyed PRIOR to calling this function.
     *
     * @param instance The instance to destroy. This value has to be nullptr or a valid instance pointer.
    */
    void destroyInstance(Instance instance);

    class InstanceT
    {
        friend result llri::createInstance(const instance_desc& desc, Instance* instance);
        friend void llri::destroyInstance(Instance instance);

    public:
        /**
         * @brief Get a vector of available adapters.
         * Using this function, you can select one or more adapters for llri::Device creation.
         * The adapters returned by this function are individual adapters and are listed separately regardless of SLI/Crossfire/Multi-GPU configuration. //TODO: SLI/Crossfire support
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if adapters is nullptr.
         * @return Otherwise it may return: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorInitializationFailed.
        */
        result enumerateAdapters(std::vector<Adapter>* adapters);

        /**
         * @brief Creates a virtual LLRI device. Device represents one or multiple adapters and enables you to allocate memory, create resources, or send commands to the adapter.
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if the instance is nullptr, if device is nullptr, if desc.adapter is nullptr, or if desc.numExtensions is more than 0 and desc.extensions is nullptr.
         * @return ErrorDeviceLost if the adapter was lost.
        */
        result createDevice(const device_desc& desc, Device* device);

        /**
         * @brief Destroy the LLRI device. This does not delete the resources allocated through the device, that responsibility remains with the user.
        */
        void destroyDevice(Device device);

    private:
        void* m_ptr = nullptr;
        void* m_debugAPI = nullptr;
        void* m_debugGPU = nullptr;

        std::map<void*, Adapter> m_cachedAdapters;
    };

    /**
     * @brief An informational enum describing the type of Adapter. The type does not directly affect how the related adapter operates, but it may correlate with performance or the availability of various features.
    */
    enum struct adapter_type
    {
        /**
         * @brief The device type is not recognized as any of the other available types.
         * This may for example be an APU, or a different form of Vulkan supported processing unit.
        */
        Other,
        /**
         * @brief GPU embedded into the host CPU.
         * Integrated GPUs typically have lower power cost but are (usually) less performant and share their memory with the host system.
        */
        Integrated,
        /**
         * @brief Separate GPU, usually connected to the host system through PCIE connectors.
        */
        Discrete,
        /**
         * @brief The adapter is a virtual node in a virtualization environment
        */
        Virtual
    };

    /**
     * @brief Converts an adapter_type to a string to aid in debug logging.
    */
    constexpr const char* to_string(const adapter_type& type)
    {
        switch (type)
        {
            case adapter_type::Other:
                return "Other";
            case adapter_type::Integrated:
                return "Integrated";
            case adapter_type::Discrete:
                return "Discrete";
            case adapter_type::Virtual:
                return "Virtual";
        }

        return "Invalid adapter_type value";
    }

    /**
     * @brief Basic information about an adapter.
     * This information is for informative purposes only and the results aren't guaranteed to be the same across APIs.
     */
    struct adapter_info
    {
        /**
         * @brief The unique ID of the hardware vendor (e.g. NVIDIA, Intel or AMD).
        */
        uint32_t vendorId;
        /**
         * @brief The ID of the adapter. This ID refers to the product type/version, meaning that if multiple of the same kind of adapters are present, this ID will be the same among them.
        */
        uint32_t adapterId;
        /**
         * @brief The name of the adapter. This string describes the adapter and usually includes the vendor name and the product type/version.
        */
        std::string adapterName;
        /**
         * @brief An informational value describing the type of Adapter. The type does not directly affect how the related adapter operates, but it may correlate with the availability of various features.
        */
        adapter_type adapterType;
    };

    /**
     * @brief Adapter features describes all features enabled or available to an Adapter.
     * Fill this structure with 0's and enable the features you wish to support, enabling all the supported features could lead to performance loss.
     * Use Adapter::queryFeatures() to get a structure with supported features to test against.
    */
    struct adapter_features
    {

    };

    class AdapterT
    {
        friend InstanceT;

    public:
        /**
         * @brief Get basic information about the Adapter. Do note that this information should be for informative purposes only and the results aren't guaranteed to be the same across APIs.
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if info is nullptr.
         * @return If the adapter was removed or lost, the operation returns ErrorDeviceRemoved.
        */
        result queryInfo(adapter_info* info) const;

        /**
         * @brief Get a structure with all supported driver/hardware features.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if features is nullptr.
         * @return ErrorIncompatibleDriver if the Adapter doesn't support the backend's requested API (either Vulkan 1.0 or DirectX 12 depending on the build).
        */
        result queryFeatures(adapter_features* features) const;

        /**
         * @brief Get the support of a given adapter extension.
         * @return true if the extension is supported, and false if it isn't.
         */
        [[nodiscard]] bool queryExtensionSupport(const adapter_extension_type& type) const;
    private:
        void* m_ptr = nullptr;
    };

    /**
     * @brief Device description to be used in Instance::createDevice().
    */
    struct device_desc
    {
        /**
         * @brief The adapter to create the instance for.
        */
        Adapter adapter;
        /**
         * @brief The enabled adapter features.
         * You should only enable features that you'll need because enabling features can disable driver optimizations.
        */
        adapter_features features;
        /**
         * @brief The number of device extensions in the device_desc::extensions array.
        */
        uint32_t numExtensions;
        /**
         * @brief The device extensions, if device_desc::numExtensions > 0, then this has to be a valid pointer to an array of DeviceExtension.
         * If numExtensions == 0, then this pointer may be nullptr.
        */
        adapter_extension* extensions;

        device_desc() = default;
        explicit device_desc(Adapter adapter, const adapter_features& features, const uint32_t& numExtensions, adapter_extension* extensions) : adapter(adapter), features(features), numExtensions(numExtensions), extensions(extensions) { }
    };

    class DeviceT
    {
        friend InstanceT;

    private:
        void* m_ptr = nullptr;
    };
}