#include <llri/llri.hpp>

#include <vector>
#include <map>
#include <vulkan/vulkan.hpp>

namespace legion::graphics::llri
{
    namespace internal
    {
        std::map<std::string, vk::LayerProperties>& queryAvailableLayers();
        std::map<std::string, vk::ExtensionProperties>& queryAvailableExtensions();
        Result mapVkResult(const vk::Result& result);
    }

    Result createInstance(const InstanceDesc& desc, Instance* instance)
    {
        if (instance == nullptr)
            return Result::ErrorInvalidUsage;
        if (desc.numExtensions > 0 && desc.extensions == nullptr)
            return Result::ErrorInvalidUsage;

        auto* result = new InstanceT();

        std::vector<const char*> layers;
        std::vector<const char*> extensions;

        auto availableLayers = internal::queryAvailableLayers();
        auto availableExtensions = internal::queryAvailableExtensions();

        void* pNext = nullptr;

        //Variables that need to be stored outside of scope
        vk::ValidationFeaturesEXT features; 
        std::vector<vk::ValidationFeatureEnableEXT> enables;

        for (uint32_t i = 0; i < desc.numExtensions; i++)
        {
            auto& extension = desc.extensions[i];
            switch(extension.type)
            {
                case InstanceExtensionType::APIValidation:
                {
                    const APIValidationEXT ext = *(APIValidationEXT*)extension.data;
                    if (ext.enable)
                        layers.push_back("VK_LAYER_KHRONOS_validation");
                    break;
                }
                case InstanceExtensionType::GPUValidation:
                {
                    const GPUValidationEXT ext = *(GPUValidationEXT*)extension.data;
                    if (ext.enable)
                    {
                        enables = {
                            vk::ValidationFeatureEnableEXT::eGpuAssisted,
                            vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot
                        };
                        features = vk::ValidationFeaturesEXT(enables, {});
                        features.pNext = pNext; //Always apply pNext backwards to simplify optional chaining
                        pNext = &features;
                    }
                    break;
                }
                default:
                {
                    llri::destroyInstance(result);
                    return Result::ErrorExtensionNotSupported;
                }
            }
        }

        vk::ApplicationInfo appInfo{ desc.applicationName, VK_MAKE_VERSION(0, 0, 0), "Legion::LLRI", VK_MAKE_VERSION(0, 0, 1), VK_HEADER_VERSION_COMPLETE };
        vk::InstanceCreateInfo ci{ {}, &appInfo, (uint32_t)layers.size(), layers.data(), (uint32_t)extensions.size(), extensions.data() };
        ci.pNext = pNext;

        vk::Instance vulkanInstance = nullptr;
        const vk::Result createResult = vk::createInstance(&ci, nullptr, &vulkanInstance);

        if (createResult != vk::Result::eSuccess)
        {
            llri::destroyInstance(result);
            return internal::mapVkResult(createResult);
        }

        result->m_ptr = vulkanInstance;
        *instance = result;
        return Result::Success;
    }

    void destroyInstance(Instance instance)
    {
        if (!instance)
            return;

        //vk validation layers aren't tangible objects and don't need manual destruction

        delete instance;
    }
}
