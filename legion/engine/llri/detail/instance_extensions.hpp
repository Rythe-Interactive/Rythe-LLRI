/**
 * @file instance_extensions.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <cstdint>

namespace LLRI_NAMESPACE
{
    /**
     * @brief Describes the kind of instance extension. This value is used in instance_extension and is used in createInstance() to recognize which value to pick from instance_extension's union.
     *
     * The support for each available instance_extension is fully **optional** (hence their name, extension), so it is **recommended** to use this enum with queryInstanceExtensionSupport() to find out if a desired extension is available prior to adding the extension to the instance_desc's extensions array.
    */
    enum struct instance_extension_type : uint8_t
    {
        /**
         * @brief Validate operations on a driver level. Driver validation often does additional parameter and context checks for the implementation's operations.
         */
        DriverValidation,
        /**
         * @brief Validate shader operations such as buffer reads/writes. This kind of validation heavily injects itself into shaders, causing a potentially significant performance impact.
        */
        GPUValidation,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = GPUValidation
    };

    /**
     * @brief Converts a instance_extension_type to a string.
     * @return The enum value as a string, or "Invalid instance_extension_type value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(instance_extension_type type);

    /**
     * @brief Enable or disable driver validation.
     * Driver validation checks for implementation-side parameters and context validity and sends the appropriate messages back if the usage is invalid or otherwise concerning.
    */
    struct driver_validation_ext
    {
        bool enable : 1;
    };

    /**
     * @brief Enable or disable GPU-side validation.
     * GPU validation validates shader operations such as buffer read/writes. Enabling this can be useful for debugging but is often associated with a significant cost.
    */
    struct gpu_validation_ext
    {
        bool enable : 1;
    };

    /**
     * @brief Describes an instance extension with its type.
     *
     * Instance extensions are additional features that are injected into the instance. They **may** activate custom behaviour in the instance, or they **may** enable the user to use functions or structures related to the extension.
     *
     * The support for each available instance_extension is fully **optional** (hence their name, extension), and thus before enabling any instance_extension, you should first query its support with queryInstanceExtensionSupport().
    */
    struct instance_extension
    {
        /**
         * @brief The type of instance extension.
         *
         * As instance extensions must be passed in an array, they need some way of storing varying data contiguously. instance_extensions do so through an unnamed union. When createInstance() attempts to implement the extension, it uses this enum value to determine which union member to pick from.
         *
         * @note Accessing incorrect union members is UB, so the union member set **must** match with the instance_extension_type passed.
        */
        instance_extension_type type;

        /**
         * @brief The instance extension's information.
         *
         * One of the values in this union must be set, and that value must be the same value as the given type.
         *
         * Passing a different structure than expected causes undefined behaviour and may result in unexplainable result values or implementation errors.
         *
         * All instance extensions are named after their enum entry, followed with _ext (e.g. instance_extension_type::DriverValidation is represented by driver_validation_ext).
        */
        union
        {
            driver_validation_ext driverValidation;
            gpu_validation_ext gpuValidation;
        };

        instance_extension() = default;
        instance_extension(instance_extension_type type, const driver_validation_ext& ext) : type(type), driverValidation(ext) { }
        instance_extension(instance_extension_type type, const gpu_validation_ext& ext) : type(type), gpuValidation(ext) { }
    };

    namespace detail
    {
        [[nodiscard]] bool queryInstanceExtensionSupport(instance_extension_type type);
    }

    /**
     * @brief Queries the support of the given extension type.
     * Support for an extension **may** depend on device configuration, hardware compatibility, or other environment factors. The support for extensions **may** differ between implementations.
     *
     * @return true if the extension is supported, and false if it isn't.
     */
    [[nodiscard]] bool queryInstanceExtensionSupport(instance_extension_type type);
}
