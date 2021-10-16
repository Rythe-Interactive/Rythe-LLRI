/**
 * @file callback.inl
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    inline std::string to_string(message_severity severity)
    {
        switch (severity)
        {
        case message_severity::Verbose:
            return "Verbose";
        case message_severity::Info:
            return "Info";
        case message_severity::Warning:
            return "Warning";
        case message_severity::Error:
            return "Error";
        case message_severity::Corruption:
            return "Corruption";
        }

        return "Invalid message_severity value";
    }

    inline std::string to_string(message_source source)
    {
        switch (source)
        {
        case message_source::API:
            return "API";
        case message_source::Implementation:
            return "Implementation";
        }

        return "Invalid message_source value";
    }
}
