#include <llri/llri.hpp>
#include <vulkan/vulkan.hpp>

#include <map>
#include <string>

namespace legion::graphics::llri
{
    namespace internal
    {
        std::map<std::string, vk::LayerProperties>& queryAvailableLayers();
        std::map<std::string, vk::ExtensionProperties>& queryAvailableExtensions();
    }

    bool queryInstanceExtensionSupport(const instance_extension_type& type)
    {
        auto layers = internal::queryAvailableLayers();

        switch (type)
        {
            case instance_extension_type::APIValidation:
            {
                return layers.find("VK_LAYER_KHRONOS_validation") != layers.end();
            }
            case instance_extension_type::GPUValidation:
            {
                return true;
            }
        }

        return false;
    }
}
