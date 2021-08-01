#include <llri/llri.hpp>
#include <llri-vk/utils.hpp>

namespace legion::graphics::llri
{
    namespace detail
    {
        [[nodiscard]] bool queryInstanceExtensionSupport(const instance_extension_type& type)
        {
            internal::lazyInitializeVolk();

            auto layers = internal::queryAvailableLayers();

            switch (type)
            {
                case instance_extension_type::APIValidation:
                {
                    return layers.find(internal::nameHash("VK_LAYER_KHRONOS_validation")) != layers.end();
                }
                case instance_extension_type::GPUValidation:
                {
                    return true;
                }
            }

            return false;
        }
    }
}
