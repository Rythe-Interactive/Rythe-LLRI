/**
 * @file utils.hpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#pragma once
#include <llri/llri.hpp>
#include <graphics/vulkan/volk.h>
#include <unordered_map>

namespace LLRI_NAMESPACE
{
    namespace internal
    {
        static bool isVolkInitialized = false;

        inline void lazyInitializeVolk()
        {
            if (!isVolkInitialized)
            {
                volkInitialize();
                isVolkInitialized = true;
            }
        }

        using layer_map = std::unordered_map<unsigned long long, VkLayerProperties>;
        using extension_map = std::unordered_map<unsigned long long, VkExtensionProperties>;

        /**
         * @brief Helps prevent loading in layers multiple times by loading them in as a static variable in this function
        */
        const layer_map& queryAvailableLayers();
        /**
         * @brief Helper function that maps extensions to their names
        */
        const extension_map& queryAvailableExtensions();

        result mapVkResult(VkResult result);

        unsigned long long nameHash(std::string name);
    }
}
