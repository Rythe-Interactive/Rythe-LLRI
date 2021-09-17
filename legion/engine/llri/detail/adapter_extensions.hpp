/**
 * @file adapter_extensions.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <cstdint>

namespace LLRI_NAMESPACE
{
    /**
     * @brief Describes the kind of adapter extension. This value is used in adapter_extension and is used in Instance::createDevice() to recognize the extension type and select the correct value from the adapter_extension's union.
     *
     * The support of individual adapter extensions is fully **optional** and often depends on hardware limitations, so this enum **should** be used with Adapter::queryExtensionSupport() to find out if an extension is available prior to adding the extension to the device_desc extension array.
    */
    enum struct adapter_extension_type : uint8_t
    {
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = 0
    };

    /**
     * @brief Converts a adapter_extension_type to a string.
     * @return The enum value as a string, or "Invalid adapter_extension_type value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(adapter_extension_type type)
    {
        switch (type)
        {
            default:
                break;
        }

        return "Invalid adapter_extension_type value";
    }

    /**
     * @brief Describes an adapter extension with its type.
     * 
     * The support of individual adapter extensions is fully **optional** and often depends on hardware limitations, so their availability **should** be queried with Adapter::queryExtensionSupport() to find out if desired extensions are available prior to adding the extensions to the device_desc extension array.
    */
    struct adapter_extension
    {
        /**
         * @brief The type of adapter extension.
         *
         * As adapter extensions must be passed in an array, they need some way of storing varying data contiguously. adapter_extensions do so through an unnamed union. When Instance::createDevice() attempts to implement the extension, it uses this enum value to determine which union member to pick from.
        */
        adapter_extension_type type;

        /**
         * @brief The adapter extension's information.
         *
         * One of the values in this union must be set, and that value must be the same value as the given type.
         *
         * Passing a different structure than expected causes undefined behaviour and may result in unexplainable result values or implementation errors.
         *
         * All adapter extensions are named after their enum entry, followed with _ext.
        */
        union
        {
            //Empty until adapter extensions are added
        };

        adapter_extension() = default;
    };
}
