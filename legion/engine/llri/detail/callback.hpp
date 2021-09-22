/**
 * @file callback.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <cstdint>
#include <string>

namespace LLRI_NAMESPACE
{
    /**
     * @brief Describes the severity of a callback message.
     * @note callback_severity is meant to be used for message filtering, and has no binding impact on the implementation's behaviour.
    */
    enum struct callback_severity : uint8_t
    {
        /**
         * @brief Extra, often excessive information about API calls, diagnostics, support, etc.
        */
        Verbose,
        /**
         * @brief Information about the implementation, operations, or resource details.
        */
        Info,
        /**
         * @brief A potential issue in the application. The issue may not be harmful, but could still lead to performance drops or unexpected behaviour.
        */
        Warning,
        /**
         * @brief Invalid (possibly fatal) API usage was detected.
        */
        Error,
        /**
         * @brief Data/memory corruption occurred.
        */
        Corruption,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Corruption
    };

    /**
     * @brief Converts a callback_severity to a string.
     * @return The enum value as a string, or "Invalid callback_severity value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(callback_severity severity);

    /**
     * @brief Describes the source of the callback message.
    */
    enum struct callback_source : uint8_t
    {
        /**
         * @brief The message came from the LLRI API. Either through validation or through other means.
        */
        API,
        /**
         * @brief The message came from the implementation.
         * Implementation validation needs to be enabled through driver_validation_ext and/or gpu_validation_ext for this kind of message to appear.
         *
         * @note This value never occurs if LLRI_DISABLE_IMPLEMENTATION_MESSAGE_POLLING is defined.
        */
        Implementation,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = Implementation
    };

    /**
     * @brief Converts a callback_source to a string.
     * @return The enum value as a string, or "Invalid callback_source value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(callback_source source);

    /**
     * @brief The callback function.
     * The callback passes numerous parameters which help classify the message's severity and source. It also passes the userData pointer that was initially passed in callback_desc.
    */
    using callback = void(
        callback_severity severity,
        callback_source source,
        const char* message,
        void* userData
        );

    namespace detail
    {
        /**
         * @brief Global user callback, not part of the public API - should not be accessed directly but instead be set by llri::setUserCallback().
        */
        inline callback* m_userCallback;
        /**
         * @brief Global user data, not part of the public API - should not be accessed directly but instead be set by llri::setUserCallback().
        */
        inline void* m_userData;

        // convenience callback functions
        inline void callUserCallback(callback_severity severity, callback_source source, const std::string& message) { if (m_userCallback) m_userCallback(severity, source, message.c_str(), m_userData); }
        inline void apiError(const std::string& func, result r, const std::string& message) { callUserCallback(callback_severity::Error, callback_source::API, func + " returned " + to_string(r) + " because " + message); }
    }

    /**
     * @brief The user callback allows the user to subscribe to callback messages so that they can write the message into their own logging system. These messages may be validation errors, implementation errors, informational messages, warnings, etc.
     *
     * The callback contains contextual information about the message, like for example its severity.
     *
     * @note Implementation messages only occur if driver_validation_ext and/or gpu_validation_ext are enabled. If no callback is set, some implementations might still output messages (Vulkan tends to print to stdout, whereas DirectX tends to print to the "Output" window in Visual Studio).
     *
     * @param userCallback The callback, the function passed must conform to the validation_callback definition. You **may** set this value to nullptr, in which case no validation messages will be sent.
     * @param userData Optional user data pointer. Not used by LLRI but it's passed around and sent along the callback.
    */
    inline void setUserCallback(callback* userCallback, void* userData = nullptr)
    {
        detail::m_userCallback = userCallback;
        detail::m_userData = userData;
    }
}
