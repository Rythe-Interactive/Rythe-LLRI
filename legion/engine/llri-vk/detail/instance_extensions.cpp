/**
 * @file instance_extensions.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

namespace llri
{
    namespace detail
    {
        [[nodiscard]] bool queryInstanceExtensionSupport(instance_extension ext)
        {
            internal::lazyInitializeVolk();

            const auto& layers = internal::queryAvailableLayers();
            const auto& extensions = internal::queryAvailableExtensions();

            switch (ext)
            {
                case instance_extension::DriverValidation:
                    return layers.find(internal::nameHash("VK_LAYER_KHRONOS_validation")) != layers.end();
                case instance_extension::GPUValidation:
                    return extensions.find(internal::nameHash("VK_EXT_validation_features")) != extensions.end();
                case instance_extension::SurfaceWin32:
                    return extensions.find(internal::nameHash("VK_KHR_win32_surface")) != extensions.end() &&
                           extensions.find(internal::nameHash("VK_KHR_surface")) != extensions.end();
                case instance_extension::SurfaceCocoa:
                    return extensions.find(internal::nameHash("VK_EXT_metal_surface")) != extensions.end() &&
                           extensions.find(internal::nameHash("VK_KHR_surface")) != extensions.end();
                case instance_extension::SurfaceXlib:
                    return extensions.find(internal::nameHash("VK_KHR_xlib_surface")) != extensions.end() &&
                            extensions.find(internal::nameHash("VK_KHR_surface")) != extensions.end();
                case instance_extension::SurfaceXcb:
                    return extensions.find(internal::nameHash("VK_KHR_xcb_surface")) != extensions.end() &&
                            extensions.find(internal::nameHash("VK_KHR_surface")) != extensions.end();
            }

            return false;
        }
    }
}
