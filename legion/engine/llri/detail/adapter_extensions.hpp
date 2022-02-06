/**
 * @file adapter_extensions.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    /**
     * @brief Adapter extensions are additional features that are injected into a Device. They **may** activate custom behaviour in the Device, or they **may** enable the user to use functions or structures related to the extension.
     *
     * The support of individual adapter extensions is fully **optional** and often depends on hardware limitations, so this enum **should** be used with Adapter::queryExtensionSupport() to find out if an extension is available prior to adding the extension to the device_desc extension array.
    */
    enum struct adapter_extension : uint8_t
    {
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = 0
    };

    /**
     * @brief Converts a adapter_extension to a string.
     * @return The enum value as a string, or "Invalid adapter_extension value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(adapter_extension ext)
    {
        (void)ext;

        return "Invalid adapter_extension value";
    }
}
