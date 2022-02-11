/**
 * @file instance_extensions.cpp
 * Copyright (c) 2021 Leon Brands, Rythe Interactive
 * SPDX-License-Identifier: MIT
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

namespace llri
{
    namespace detail
    {
        [[nodiscard]] bool queryInstanceExtensionSupport(instance_extension ext)
        {
            detail::lazyInitializeVolk();

            const auto& layers = detail::queryAvailableLayers();
            const auto& extensions = detail::queryAvailableExtensions();

            switch (ext)
            {
                case instance_extension::DriverValidation:
                    return layers.find(detail::nameHash("VK_LAYER_KHRONOS_validation")) != layers.end();
                case instance_extension::GPUValidation:
                    return extensions.find(detail::nameHash("VK_EXT_validation_features")) != extensions.end();
                case instance_extension::SurfaceWin32:
                    return extensions.find(detail::nameHash("VK_KHR_win32_surface")) != extensions.end() &&
                           extensions.find(detail::nameHash("VK_KHR_surface")) != extensions.end();
                case instance_extension::SurfaceCocoa:
                    return extensions.find(detail::nameHash("VK_EXT_metal_surface")) != extensions.end() &&
                           extensions.find(detail::nameHash("VK_KHR_surface")) != extensions.end();
                case instance_extension::SurfaceXlib:
                    return extensions.find(detail::nameHash("VK_KHR_xlib_surface")) != extensions.end() &&
                            extensions.find(detail::nameHash("VK_KHR_surface")) != extensions.end();
                case instance_extension::SurfaceXcb:
                    return extensions.find(detail::nameHash("VK_KHR_xcb_surface")) != extensions.end() &&
                            extensions.find(detail::nameHash("VK_KHR_surface")) != extensions.end();
            }

            return false;
        }
    }
}
