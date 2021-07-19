#pragma once
#include <llri/llri.hpp>
#include <vulkan/vulkan.hpp>
#include <unordered_map>

namespace legion::graphics::llri
{
    namespace internal
    {
        using layer_map = std::unordered_map<unsigned long long, vk::LayerProperties>;
        using extension_map = std::unordered_map<unsigned long long, vk::ExtensionProperties>;

        /**
         * @brief Helps prevent loading in layers multiple times by loading them in as a static variable in this function
        */
        const layer_map& queryAvailableLayers();
        /**
         * @brief Helper function that maps extensions to their names
        */
        const extension_map& queryAvailableExtensions();

        result mapVkResult(const vk::Result& result);

        unsigned long long nameHash(std::string name);
    }
}
