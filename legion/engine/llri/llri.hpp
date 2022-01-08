/**
 * @file llri.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <cmath>
#include <iostream>

#include <unordered_set>
#include <unordered_map>

#include <type_traits>

#if defined(DOXY_EXCLUDE)
/**
 * @def LLRI_DISABLE_VALIDATION
 * @brief Defining LLRI_DISABLE_VALIDATION disables all LLRI validation.
 * This applies to all validation done by the LLRI API (not the implementation), such as nullptr checks on parameters.
 *
 * @note Disabling LLRI validation **may** cause API or implementation runtime errors if incorrect parameters are passed, but the reduced checks could improve performance.
 * @note Disabling LLRI validation means that LLRI **will not** return result::ErrorInvalidUsage and result::ErrorDeviceLost where it normally would if incorrect parameters are passed, but the implementation **may** still return these codes if it fails to operate.
 */
#define LLRI_DISABLE_VALIDATION

 /**
  * @def LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
  * @brief Defining LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING disables all implementation message polling.
  * Implementation message polling can be costly and disabling it could improve performance, but doing so causes implementation messages to not be forwarded to the message callback.
  *
  * @note Disabling implementation message polling is not guaranteed to prevent implementations from sending messages through other means. Drivers often have their own way of forwarding messages and it's very possible that messages end up in stdout or visual studio's output window.
  */
#define LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING
#endif

/**
 * @def LLRI_TIMEOUT_MIN
 * @brief Timeout immediately. If waiting needs to occur then the wait function will immediately return result::Timeout, if no waiting needs to occur, the waiting function returns result::Success.
 */
#define LLRI_TIMEOUT_MIN 0u
/**
 * @def LLRI_TIMEOUT_MAX
 * @brief The maximum possible timeout. Using this **may** disable timeout entirely in some implementations.
 */
#define LLRI_TIMEOUT_MAX 0xFFFFFFFFu

namespace llri
{
    /**
     * @brief Result codes for LLRI operations.
     * 
     * 
     * Most LLRI operations return result codes. These result codes provide information about the operation's execution status.
     * Operations that execute properly **can** return result::Success, but they **may** return any of the other non-error result codes.
     * If an operation fails, it **must** return a failing result value, which **may** be result::ErrorUnknown or a more specific appropriate failing result value.
     * 
     * @note Codes prefixed with "Error" imply that the operation failed fatally. This **may** mean that further action to recover the application's state is required by the user.
     *
     * @note Result codes may not provide enough information, so consider using llri::setMessageCallback() to get additional information.
    */
    enum struct result : uint8_t
    {
        /**
         * @brief The operation executed properly.
        */
        Success = 0,
        /**
         * @brief The operation's execution time exceeded a user-defined timeout.
        */
        Timeout,
        /**
         * @brief A fence has not yet completed.
        */
        NotReady,
        /**
         * @brief The operation failed fatally, but no error was specified.
         * The implementation **may** return this value if it can't map an error code to an LLRI result code.
        */
        ErrorUnknown,
        /**
         * @brief The usage of the operation was invalid.
         * LLRI validation returns this result code whenever its validation fails, but implementations **can** return the code too.
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
         * @brief An operation with an extension was used but the extension wasn't enabled.
        */
        ErrorExtensionNotEnabled,
        /**
         * @brief A passed node mask was not a valid node mask in the device. This may occur if the node mask had a positive bit set that was higher than the number of nodes represented in the device.
        */
        ErrorInvalidNodeMask,
        /**
         * @brief An object interacted with another object, where the two objects did not have matching node masks.
        */
        ErrorIncompatibleNodeMask,
        /**
         * @brief The operation caused the device to become non-responsive for a long period of time. This is often caused by badly formed commands sent by the application.
        */
        ErrorDeviceHung,
        /**
         * @brief A device **may** be lost after invalid API usage causes fatal errors that the device can not recover from.
         * The device becomes invalid and must be destroyed and recreated.
         *
         * After a device loss, the application **should** reiterate over available Adapters as the previously used Adapter **may** have become invalid.
        */
        ErrorDeviceLost,
        /**
         * @brief The video card has been physically removed from the system. The device becomes invalid and must be destroyed and recreated. For this, the application **should** reiterate over available Adapters as the previously used Adapter **may** have become invalid.
        */
        ErrorDeviceRemoved,
        /**
         * @brief A driver error occurred. After this, the device will be put into the device lost state and will become invalid. See result::ErrorDeviceLost for more information on device loss.
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
         * @brief Initialization of an object failed because of implementation specific reasons.
        */
        ErrorInitializationFailed,
        /**
         * @brief The implementation is not supported by the driver.
        */
        ErrorIncompatibleDriver,
        /**
         * @brief A resource was in a different state than the operation expected it to be in.
        */
        ErrorInvalidState,
        /**
         * @brief An API defined creation/allocation limit was exceeded.
        */
        ErrorExceededLimit,
        /**
         * @brief An object was already occupied with another operation and thus couldn't be used for the submitted operation
        */
        ErrorOccupied,
        /**
         * @brief An operation attempted to wait on a Fence that has not been signaled.
        */
        ErrorNotSignaled,
        /**
         * @brief An operation attempted to signal a Fence that had already been signaled.
        */
        ErrorAlreadySignaled,
        /**
         * @brief An invalid or incompatible format was used.
        */
        ErrorInvalidFormat,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = ErrorInvalidFormat
    };

    /**
     * @brief Converts a result to a string.
     * @return The enum value as a string, or "Invalid result value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(result r);

    /**
    * @brief Names the implementation.
    */
    enum struct implementation : uint8_t
    {
        /**
        * @brief The cross-platform Vulkan API. 
        */
        Vulkan,
        /**
        * @brief Microsoft's DirectX 12 API.
        */
        DirectX12,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = DirectX12
    };

    /**
     * @brief Converts an implementation to a string.
     * @return The enum value as a string, or "Invalid implementation value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(implementation impl);

    /**
    * @brief Query the currently linked implementation.
    */
    [[nodiscard]] implementation getImplementation();
}

// ReSharper disable CppUnusedIncludeDirective

#include <llri/detail/validation.hpp>
#include <llri/detail/flags.hpp>
#include <llri/detail/callback.hpp>

#include <llri/detail/instance.hpp>
#include <llri/detail/instance_extensions.hpp>

#include <llri/detail/adapter.hpp>
#include <llri/detail/adapter_extensions.hpp>

#include <llri/detail/queue.hpp>
#include <llri/detail/device.hpp>

#include <llri/detail/resource.hpp>

#include <llri/detail/command_group.hpp>
#include <llri/detail/command_list.hpp>

#include <llri/detail/fence.hpp>
#include <llri/detail/semaphore.hpp>

#include <llri/detail/llri.inl>
