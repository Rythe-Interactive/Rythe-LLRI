/**
 * @file callback.hpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    /**
     * @brief Describes the severity of a callback message.
     * @note message_severity is meant to be used for message filtering, and has no binding impact on the implementation's behaviour.
    */
    enum struct message_severity : uint8_t
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
     * @brief Converts a message_severity to a string.
     * @return The enum value as a string, or "Invalid message_severity value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(message_severity severity);

    /**
     * @brief Describes the source of the callback message.
    */
    enum struct message_source : uint8_t
    {
        /**
         * @brief The message came from the LLRI API. Either through validation or through other means.
        */
        API,
        /**
         * @brief The message came from the implementation.
         * Implementation validation needs to be enabled through instance_extension::DriverValidation and/or GPUValidation for this kind of message to appear.
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
     * @brief Converts a message_severity to a string.
     * @return The enum value as a string, or "Invalid message_severity value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(message_source source);

    /**
     * @brief The callback function.
     * The callback passes numerous parameters which help classify the message's severity and source. It also passes the userData pointer that was initially passed in callback_desc.
    */
    using message_callback = void(
        message_severity severity,
        message_source source,
        const char* message,
        void* userData
        );

    namespace detail
    {
        /**
         * @brief Global message callback, not part of the public API - should not be accessed directly but instead be set through llri::setMessageCallback().
        */
        inline message_callback* m_messageCallback;
        /**
         * @brief Global user data, not part of the public API - should not be accessed directly but instead be set through llri::setMessageCallback().
        */
        inline void* m_userData;

        // convenience callback functions
        inline void callUserCallback(message_severity severity, message_source source, const std::string& message) { if (m_messageCallback) m_messageCallback(severity, source, message.c_str(), m_userData); }
        inline void apiError(const std::string& func, result r, const std::string& message) { callUserCallback(message_severity::Error, message_source::API, func + " returned " + to_string(r) + " because " + message); }
        inline void apiError(const std::string& func, void* dummy, const std::string& message) { callUserCallback(message_severity::Error, message_source::API, func + " returned " + std::to_string(reinterpret_cast<uint64_t>(dummy)) + " because " + message); }
        inline void apiWarning(const std::string& func, const std::string& message) { callUserCallback(message_severity::Warning, message_source::API, "in " + func + ": " + message); }
    }

    /**
     * @brief The message callback allows the user to subscribe to callback messages so that they can write the message into their own logging system. These messages may be validation errors, implementation errors, informational messages, warnings, or anything else that the API or its implementation wishes to forward.
     *
     * The callback contains contextual information about the message, like for example its severity.
     *
     * @note Implementation messages only occur if instance_extension::DriverValidation and/or GPUValidation are enabled. If no message callback is set, some implementations might still output messages (Vulkan tends to print to stdout, whereas DirectX tends to print to the "Output" window in Visual Studio).
     *
     * @param callback The callback, the function passed must conform to the message_callback definition. You **may** set this value to nullptr, in which case no messages are sent.
     * @param userData Optional user data pointer. Not used by LLRI but it's passed around and sent along the callback.
    */
    inline void setMessageCallback(message_callback* callback, void* userData = nullptr)
    {
        detail::m_messageCallback = callback;
        detail::m_userData = userData;
    }
}
