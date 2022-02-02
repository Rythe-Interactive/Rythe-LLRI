/**
 * @file instance_extensions.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    /**
     * @brief Instance extensions are additional features that are injected into the instance. They **may** activate custom behaviour in the instance, or they **may** enable the user to use functions or structures related to the extension.
     *
     * The support for each available instance_extension is fully **optional** (hence their name - extension), so a user **should** query support using queryInstanceExtensionSupport().
    */
    enum struct instance_extension : uint8_t
    {
        /**
         * @brief Driver validation checks for implementation-side parameters and context validity and sends the appropriate messages back if the usage is invalid or otherwise concerning.
         */
        DriverValidation,
        /**
         * @brief GPU validation validates shader operations such as buffer read/writes. Enabling this can be useful for debugging but is often associated with a significant performance cost.
        */
        GPUValidation,
        /**
         * @brief Create a SurfaceEXT object from a Win32 HWND window.
        */
        SurfaceWin32,
        /**
         * @brief Create a SurfaceEXT object from a Cocoa window, using CAMetalLayer.
         */
        SurfaceCocoa,
        /**
         * @brief Create a SurfaceEXT object from an Xlib display connection and window
         */
        SurfaceXlib,
        /**
         * @brief Create a SurfaceEXT object from an XCB connection & XCB window
         */
        SurfaceXcb,
        /**
         * @brief The highest value in this enum.
        */
        MaxEnum = SurfaceXcb
    };

    /**
     * @brief Converts a instance_extension to a string.
     * @return The enum value as a string, or "Invalid instance_extension value" if the value was not recognized as an enum member.
    */
    inline std::string to_string(instance_extension ext);

    namespace detail
    {
        [[nodiscard]] bool queryInstanceExtensionSupport(instance_extension ext);
    }

    /**
     * @brief Queries the support of the given extension.
     * Support for an extension **may** depend on system configuration, hardware compatibility, or other environment factors. The support for extensions **may** differ between implementations.
     *
     * @return true if the extension is supported, and false if it isn't.
     */
    [[nodiscard]] bool queryInstanceExtensionSupport(instance_extension ext);
}
