/**
 * @file instance_extensions.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

namespace LLRI_NAMESPACE
{
    namespace detail
    {
        [[nodiscard]] bool queryInstanceExtensionSupport(instance_extension_type type)
        {
            internal::lazyInitializeVolk();

            auto& layers = internal::queryAvailableLayers();
            auto& extensions = internal::queryAvailableExtensions();

            switch (type)
            {
                case instance_extension_type::DriverValidation:
                {
                    return layers.find(internal::nameHash("VK_LAYER_KHRONOS_validation")) != layers.end();
                }
                case instance_extension_type::GPUValidation:
                {
                    return true;
                }
                case instance_extension_type::AdapterNodes:
                {
                    return extensions.find(internal::nameHash("VK_KHR_device_group_creation")) != extensions.end();
                }
            }

            return false;
        }
    }
}
