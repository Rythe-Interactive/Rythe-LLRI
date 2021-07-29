#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#if defined(DOXY_EXCLUDE)
/**
 * @def LLRI_DISABLE_VALIDATION
 * @brief Before including LLRI, define LLRI_DISABLE_VALIDATION to disable all LLRI validation.
 * This applies to all validation done by LLRI (not internal API validation), such as nullptr checks on parameters.
 * Disabling LLRI validation may cause API runtime errors if incorrect parameters are passed, but the reduced checks could improve performance.
 *
 * Disabling LLRI validation will also mean that LLRI will not return ErrorInvalidUsage and ErrorDeviceLost where it normally would if incorrect parameters are passed, but the internal API may still return these codes if it fails to operate.
 */
#define LLRI_DISABLE_VALIDATION

 /**
  * @def LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING
  * @brief Before including LLRI, define LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING to disable internal API message polling.
  * Internal API message polling can be costly and disabling it can help improve performance, but internal API messages might not be forwarded.
  */
#define LLRI_DISABLE_INTERNAL_API_MESSAGE_POLLING
#endif

namespace legion::graphics::llri
{
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
}

#include <llri/detail/instance.hpp>
#include <llri/detail/instance_extensions.hpp>

#include <llri/detail/adapter.hpp>
#include <llri/detail/adapter_extensions.hpp>

#include <llri/detail/device.hpp>

// ReSharper disable once CppUnusedIncludeDirective
#include <llri/detail/llri.inl>
