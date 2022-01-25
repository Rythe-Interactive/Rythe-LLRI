/**
 * @file surface_ext.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp> // unnecessary but helps intellisense

namespace llri
{
    /**
     * @brief Describes how a SurfaceEXT object should be created with a Win32 HWND window.
    */
    struct surface_win32_desc_ext
    {
        /**
         * @brief Win32 HINSTANCE for the window to associate the surface with.
         *
         * @note Valid usage (ErrorInvalidUsage): **Must** be a valid non-null Win32 HINSTANCE.
        */
        void* hinstance;
        /**
         * @brief The HWND window.
         *
         * @note Valid usage (ErrorInvalidUsage): **Must** be a valid non-null HWND window.
        */
        void* hwnd;
    };

    /**
     * @brief Describes how a SurfaceEXT object should be created with an NSWindow window.
     */
    struct surface_metal_desc_ext
    {
        /**
         * @brief NSWindow, for which a CAMetalLayer will be created (or obtained if it already exists).
         *
         * @note Valid usage (ErrorInvalidUsage): **Must** be a valid non-null pointer to a NSWindow.
         */
        void* nsWindow;
    };

    /**
     * @brief LLRI Surfaces are created from platform-dependent information.
     * They are the link between platforms and the abstract LLRI API.
     * A Surface represents a window or another platform-specific structure that (usually) supports drawing to a screen.
    */
    class SurfaceEXT
    {
        friend class Instance;
        friend class Device;
        friend class Adapter;

    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        SurfaceEXT() = default;
        ~SurfaceEXT() = default;

        void* m_ptr = nullptr;
    };
}
