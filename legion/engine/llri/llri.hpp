#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <llri/instance_extensions.hpp>
#include <llri/adapter_extensions.hpp>

#ifndef LLRI_ENABLE_VALIDATION
/**
 * @brief Before including LLRI, define LLRI_ENABLE_VALIDATION as 0 to disable all LLRI validation.
 * This applies to all validation done by LLRI (not internal API validation), such as nullptr checks on parameters.
 * Disabling LLRI validation may cause API runtime errors if incorrect parameters are passed, but the reduced checks could improve performance.
 *
 * Disabling LLRI validation will also mean that LLRI will not return ErrorInvalidUsage and ErrorDeviceLost where it normally would if incorrect parameters are passed, but the internal API may still return these codes if it fails to operate.
 */
#define LLRI_ENABLE_VALIDATION 1
#endif

#ifndef LLRI_ENABLE_INTERNAL_API_MESSAGE_POLLING
 /**
  * @brief Before including LLRI, define LLRI_ENABLE_INTERNAL_API_MESSAGE_POLLING as 0 to disable internal API message polling.
  * Internal API message polling can be costly and disabling it can help improve performance, but internal API messages might not be forwarded.
  */
#define LLRI_ENABLE_INTERNAL_API_MESSAGE_POLLING 1
#endif

namespace legion::graphics::llri
{
    class Instance;
    class Adapter;
    class Device;

    struct instance_desc;
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
    constexpr const char* to_string(const result& result);

    /**
     * @brief Describes the severity of a callback message.
    */
    enum class validation_callback_severity
    {
        /**
         * @brief Provides extra, often excessive information about API calls, diagnostics, support, etc.
        */
        Verbose,
        /**
         * @brief Provides information about API calls or resource details.
        */
        Info,
        /**
         * @brief Describes a potential issue in the application. The issue may not be harmful, but could still lead to performance drops or unexpected behaviour.
        */
        Warning,
        /**
         * @brief Fatal invalid API usage was detected.
        */
        Error,
        /**
         * @brief Data/memory corruption occurred.
        */
        Corruption
    };

    /**
     * @brief Converts a validation_callback_severity to a string to aid in debug logging.
    */
    constexpr const char* to_string(const validation_callback_severity& severity);

    /**
     * @brief Describes the source of the validation callback message.
    */
    enum class validation_callback_source
    {
        /**
         * @brief The message came from the LLRI API directly.
         * LLRI validation does basic parameter checks to make sure that the API doesn't crash internally.
        */
        Validation,
        /**
         * @brief The message came from the internal API.
         * Internal API validation needs to be enabled through APIValidationEXT and/or GPUValidationEXT for this kind of message to appear.
        */
        InternalAPI
    };

    /**
     * @brief Converts a validation_callback_source to a string to aid in debug logging.
    */
    constexpr const char* to_string(const validation_callback_source& source);

    /**
     * @brief The debug callback function, this function passes a severity (info, warning, error, etc), a source (LLRI validation or Internal API message), the message, and some userdata that can be set in the validation_callback_desc.
    */
    using FnValidationCallback = void (
        const validation_callback_severity& severity,
        const validation_callback_source& source,
        const char* message,
        void* userData
        );

    /**
     * @brief The validation callback allows the user to subscribe to validation messages so that they can write the message into their own logging system. 
     *
     * The callback contains contextual information about the message, like for example its severity.
     * The callback may be used for both LLRI validation and internal API validation. The callback will poll messages from the internal API (e.g. Vulkan's debug utils, and DirectX's info queue)
     *
     * Internal API messages only occur if api_validation_ext and/or gpu_validation_ext are enabled. If no callback is set, some APIs might still output messages (Vulkan tends to print to the console, whereas DirectX tends to print to the "Output" window in Visual Studio).
    */
    struct validation_callback_desc
    {
        /**
         * @brief The callback, the function passed must conform to the FnValidationCallback definition.
         * This value CAN be nullptr, in which case no validation messages will be sent.
        */
        FnValidationCallback* callback;
        /**
         * @brief Optional user data pointer. Not used internally by the API but it's passed around and sent along the callback.
        */
        void* userData;

        /**
         * @brief Convenience operator used internally to call the callback.
        */
        void operator ()(const validation_callback_severity& severity, const validation_callback_source& source, const char* message) const { callback(severity, source, message, userData); }
    };

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
        /**
         * @brief Describes the optional validation callback. callbackDesc.callback can be nullptr in which case no callbacks will be sent.
         *
         * Callbacks may or may not be sent depending on the parameters used. If LLRI_ENABLE_VALIDATION is set to 0, no LLRI validation messages will be sent. If LLRI_ENABLE_INTERNAL_API_MESSAGE_POLLING is set to 0, then no internal API messages will be forwarded.
         *
         * Furthermore, to enable internal API messages, api_validation_ext and/or gpu_validation_ext should be enabled and part of the extensions array.
        */
        validation_callback_desc callbackDesc;
    };

    /**
     * @brief Internal functions, don't use outside of the API.
    */
    namespace detail
    {
        result impl_createInstance(const instance_desc& desc, Instance** instance, const bool& enableInternalAPIMessagePolling);
        void impl_destroyInstance(Instance* instance);

        /**
         * @brief Polls API messages, called if LLRI_ENABLE_INTERNAL_API_MESSAGE_POLLING is set to 1.
         * Used internally only
         * @param validation The validation function / userdata
         * @param messenger This value may differ depending on the function that is calling it, the most relevant messenger will be picked.
        */
        void impl_pollAPIMessages(const validation_callback_desc& validation, void* messenger);
    }

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
    result createInstance(const instance_desc& desc, Instance** instance);

    /**
     * @brief Destroys the given instance and its directly related internal resources (debug info etc).
     * All resources created through the instance must be destroyed PRIOR to calling this function.
     *
     * @param instance The instance to destroy. This value has to be nullptr or a valid instance pointer.
    */
    void destroyInstance(Instance* instance);

    /**
     * @brief Instance is the center of the application and is used to create most other API objects.
     * Usually only a single instance of Instance exists within an application, but if so desired, multiple Instance instances are supported.
    */
    class Instance
    {
        friend result detail::impl_createInstance(const instance_desc& desc, Instance** instance, const bool& enableInternalAPIMessagePolling);
        friend void detail::impl_destroyInstance(Instance* instance);

        friend result llri::createInstance(const instance_desc& desc, Instance** instance);
        friend void llri::destroyInstance(Instance* instance);

    public:
        /**
         * @brief Get a vector of available adapters.
         * Using this function, you can select one or more adapters for llri::Device creation.
         * The adapters returned by this function are individual adapters and are listed separately regardless of SLI/Crossfire/Multi-GPU configuration. //TODO: SLI/Crossfire support
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if adapters is nullptr.
         * @return Otherwise it may return: ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorInitializationFailed.
        */
        result enumerateAdapters(std::vector<Adapter*>* adapters);

        /**
         * @brief Creates a virtual LLRI device. Device represents one or multiple adapters and enables you to allocate memory, create resources, or send commands to the adapter.
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if the instance is nullptr, if device is nullptr, if desc.adapter is nullptr, or if desc.numExtensions is more than 0 and desc.extensions is nullptr.
         * @return ErrorDeviceLost if the adapter was lost.
        */
        result createDevice(const device_desc& desc, Device** device) const;

        /**
         * @brief Destroy the LLRI device. This does not delete the resources allocated through the device, that responsibility remains with the user.
        */
        void destroyDevice(Device* device) const;

    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Instance() = default;
        ~Instance() = default;

        void* m_ptr = nullptr;
        void* m_debugAPI = nullptr;
        void* m_debugGPU = nullptr;

        validation_callback_desc m_validationCallback;
        void* m_validationCallbackMessenger = nullptr; //Allows API to store their callback messenger if needed

        std::map<void*, Adapter*> m_cachedAdapters;

        result impl_enumerateAdapters(std::vector<Adapter*>* adapters);
        result impl_createDevice(const device_desc& desc, Device** device) const;
        void impl_destroyDevice(Device* device) const;
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
    constexpr const char* to_string(const adapter_type& type);

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

    /**
     * @brief A compatible adapter (GPU, APU, IGPU, etc.).
     * This handle is created and owned by the Instance, the user is not responsible for destroying it.
    */
    class Adapter
    {
        friend Instance;
        friend result detail::impl_createInstance(const instance_desc&, Instance**, const bool&);
        friend void detail::impl_destroyInstance(Instance*);

    public:
        /**
         * @brief Get basic information about the Adapter. Do note that this information should be for informative purposes only and the results aren't guaranteed to be the same across APIs.
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if info is nullptr.
         * @return ErrorDeviceLost If the adapter was removed or lost.
        */
        result queryInfo(adapter_info* info) const;

        /**
         * @brief Get a structure with all supported driver/hardware features.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage if features is nullptr.
         * @return ErrorIncompatibleDriver if the Adapter doesn't support the backend's requested API (either Vulkan 1.0 or DirectX 12 depending on the build).
         * @return ErrorDeviceLost If the adapter was removed or lost.
        */
        result queryFeatures(adapter_features* features) const;

        /**
         * @brief Get the support of a given adapter extension.
         *
         * @return Success upon correct execution of the operation.
         * @return ErrorInvalidUsage If supported is nullptr.
         * @return ErrorDeviceLost If the adapter was removed or lost.
         */
        result queryExtensionSupport(const adapter_extension_type& type, bool* supported) const;
    private:
        //Force private constructor/deconstructor so that only instance can manage lifetime
        Adapter() = default;
        ~Adapter() = default;

        void* m_ptr = nullptr;
        validation_callback_desc m_validationCallback;
        void* m_validationCallbackMessenger = nullptr;

        result impl_queryInfo(adapter_info* info) const;
        result impl_queryFeatures(adapter_features* features) const;
        result impl_queryExtensionSupport(const adapter_extension_type& type, bool* supported) const;
    };

    /**
     * @brief Device description to be used in Instance::createDevice().
    */
    struct device_desc
    {
        /**
         * @brief The adapter to create the instance for.
        */
        Adapter* adapter;
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
    };

    /**
     * @brief A device is a virtual representation of an adapter and can create/destroy resources for the said adapter.
     */
    class Device
    {
        friend Instance;
    public:

    private:
        //Force private constructor/deconstructor so that only create/destroy can manage lifetime
        Device() = default;
        ~Device() = default;

        void* m_ptr = nullptr;
        validation_callback_desc m_validationCallback;
        void* m_validationCallbackMessenger = nullptr;
    };
}

// ReSharper disable once CppUnusedIncludeDirective
#include <llri/llri_impl.hpp>
