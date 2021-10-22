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
            }

            return false;
        }
    }
}
