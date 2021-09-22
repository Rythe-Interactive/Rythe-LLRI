/**
 * @file callback.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> //Recursive include technically not necessary but helps with intellisense

namespace LLRI_NAMESPACE
{
    inline std::string to_string(callback_severity severity)
    {
        switch (severity)
        {
        case callback_severity::Verbose:
            return "Verbose";
        case callback_severity::Info:
            return "Info";
        case callback_severity::Warning:
            return "Warning";
        case callback_severity::Error:
            return "Error";
        case callback_severity::Corruption:
            return "Corruption";
        }

        return "Invalid validation_callback_severity value";
    }

    inline std::string to_string(callback_source source)
    {
        switch (source)
        {
        case callback_source::API:
            return "API";
        case callback_source::Implementation:
            return "Implementation";
        }

        return "Invalid validation_callback_source value";
    }
}
