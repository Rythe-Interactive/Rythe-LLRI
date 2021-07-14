#pragma once
#include <cstdint>
#include <llri/instance_extensions.hpp>

namespace legion::graphics::llri
{
    /**
     * @brief Informative result values for llri operations.
     *
     * Operations that execute properly will return Result::Success, or if they fail they will pick the appropriate failing Result value. Note that some Result values are prefixed with "Error", implying that their Result value was fatal and can not be recovered from. Failures without the "Error" prefix are often soft failures that might for example be caused by user-defined timeouts.
    */
    enum class Result
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
     * @brief Converts a Result to a string to aid in debug logging.
    */
    constexpr const char* to_string(const Result& result)
    {
        switch (result)
        {
        case Result::Success:
            return "Success";
        case Result::Timeout:
            return "Timeout";
        case Result::ErrorUnknown:
            return "ErrorUnknown";
        case Result::ErrorInvalidUsage:
            return "ErrorInvalidUsage";
        case Result::ErrorFeatureNotSupported:
            return "ErrorFeatureNotSupported";
        case Result::ErrorExtensionNotSupported:
            return "ErrorExtensionNotSupported";
        case Result::ErrorDeviceHung:
            return "ErrorDeviceHung";
        case Result::ErrorDeviceLost:
            return "ErrorDeviceLost";
        case Result::ErrorDeviceRemoved:
            return "ErrorDeviceRemoved";
        case Result::ErrorDriverFailure:
            return "ErrorDriverFailure";
        }

        return "Invalid Result value";
    }

    struct InstanceT;

    /**
     * @brief Handle to the core llri::Instance. Usually only a single instance of Instance exists within an application, but if so desired, multiple Instance instances are supported.
     * Instance is the center of the application and is used to create most other API objects.
    */
    typedef InstanceT* Instance;

    /**
     * @brief Instance description to be used in llri::createInstance().
    */
    struct InstanceDesc
    {
        /**
         * @brief The number of instance extensions in the InstanceDesc::extensions array.
        */
        uint32_t numExtensions;
        /**
         * @brief The instance extensions, if InstanceDesc::numExtensions > 0, then this has to be a valid pointer to an array of InstanceExtension.
         * If numExtensions == 0, then this pointer may be nullptr.
        */
        InstanceExtension* extensions;
        /**
         * @brief Sets the name of the application in internal API if applicable.
         * This is not guaranteed but is known to at least apply to Vulkan.
        */
        const char* applicationName;

        InstanceDesc() = default;
        InstanceDesc(const uint32_t& numExtensions, InstanceExtension* extensions, const char* applicationName) : numExtensions(numExtensions), extensions(extensions), applicationName(applicationName) { }
    };

    /**
     * @brief Create an llri Instance.
     * Like with all API objects, the user is responsible for destroying the instance again using destroyInstance().
     * @param desc The description of the instance.
     * @param instance Must be a valid pointer to an Instance variable.
     *
     * @returns Success upon correct execution of operation. Otherwise it may return: ErrorInvalidUsage, ErrorExtensionNotSupported, ErrorOutOfHostMemory, ErrorOutOfDeviceMemory, ErrorInitializationFailed, ErrorIncompatibleDriver.
    */
    Result createInstance(const InstanceDesc& desc, Instance* instance);

    /**
     * @brief Destroys the given instance and its directly related internal resources (debug info etc).
     * All resources created through the instance must be destroyed PRIOR to calling this function.
     *
     * @param instance The instance to destroy. This value has to be nullptr or a valid instance pointer.
    */
    void destroyInstance(Instance instance);

    struct InstanceT
    {
        friend Result llri::createInstance(const InstanceDesc& desc, Instance* instance);
        friend void llri::destroyInstance(Instance instance);

    private:
        void* m_ptr = nullptr;
        void* m_debugAPI = nullptr;
        void* m_debugGPU = nullptr;
    };        
}
