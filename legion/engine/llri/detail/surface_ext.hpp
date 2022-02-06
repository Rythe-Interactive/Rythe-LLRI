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
     * @brief Describes how a SurfaceEXT object should be created with a Cocoa window.
     */
    struct surface_cocoa_desc_ext
    {
        /**
         * @brief Cocoa NS window, for which a CAMetalLayer will be created (or obtained if it already exists).
         *
         * @note Valid usage (ErrorInvalidUsage): **Must** be a valid non-null pointer to a NSWindow.
         */
        void* nsWindow;
    };

    /**
     * @brief Describes how a SurfaceEXT object should be created with an XLib Display connection and Window.
     */
    struct surface_xlib_desc_ext
    {
        /**
         * @brief An Xlib display connection connecting to the X server.
         *
         * @note Valid usage (ErrorInvalidUsage): **Must** be a valid non-null pointer to an Xlib Display connection.
         */
        void* display;
        /**
         * @brief An Xlib window handle.
         *
         * @note Valid usage (ErrorInvalidUsage): **Must** be a valid Xlib window (not 0).
         */
        uint32_t window;
    };

    /**
     * @brief Describes how a SurfaceEXT object should be created with an XCB connection and window ID.
     */
    struct surface_xcb_desc_ext
    {
        /**
         * @brief A pointer to an XCB connection pointing to the X server.
         *
         * @note Valid usage (ErrorInvalidUsage): **Must** be a valid non-null pointer to an XCB connection.
         */
        void* connection;
        /**
         * @brief An XCB window handle.
         *
         * @note Valid usage (ErrorInvalidUsage): **Must** be a valid XCB ID (not 0).
         */
        uint32_t window;
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

    public:
        using native_surface_ext = void;

        /**
         * @brief Gets the native SurfaceEXT  pointer, which depending on the llri::getImplementation() is a pointer to the following:
         *
         * DirectX12: HWND
         * Vulkan: VkSurfaceEXT
         */
        [[nodiscard]] native_surface_ext* getNative() const
        {
            return m_ptr;
        }
        
    private:
        // Force private constructor/deconstructor so that only create/destroy can manage lifetime
        SurfaceEXT() = default;
        ~SurfaceEXT() = default;

        native_surface_ext* m_ptr = nullptr;
    };
}
