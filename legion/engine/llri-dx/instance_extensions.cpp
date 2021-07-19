#include <graphics/directx/d3d12sdklayers.h>
#include <llri/llri.hpp>

namespace legion::graphics::llri
{
    bool queryInstanceExtensionSupport(const instance_extension_type& type)
    {
        switch (type)
        {
            case instance_extension_type::APIValidation:
            {
                ID3D12Debug* temp = nullptr;
                return SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&temp)));
            }
            case instance_extension_type::GPUValidation:
            {
                ID3D12Debug1* temp = nullptr;
                return SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&temp)));
            }
        }

        return false;
    }

    namespace internal
    {
        /**
         * @brief Enables D3D12 API validation layers where requested.
         * @return If the needed debug interface is not found/supported, the function returns result::ErrorExtensionNotSupported.
        */
        result createAPIValidationEXT(const APIValidationEXT& ext, void** output)
        {
            if (ext.enable)
            {
                ID3D12Debug* debugAPI = nullptr;
                if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugAPI))))
                {
                    debugAPI->EnableDebugLayer();
                    *output = debugAPI;
                }
                else
                    return result::ErrorExtensionNotSupported;
            }
            return result::Success;
        }

        /**
         * @brief Enables D3D12 GPU validation layers where requested.
         * @return If the needed debug interface is not found/supported, the function returns result::ErrorExtensionNotSupported.
        */
        result createGPUValidationEXT(const GPUValidationEXT& ext, void** output)
        {
            if (ext.enable)
            {
                ID3D12Debug1* debugGPU = nullptr;
                if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugGPU))))
                {
                    debugGPU->SetEnableGPUBasedValidation(true);
                    debugGPU->SetEnableSynchronizedCommandQueueValidation(true);
                    *output = debugGPU;
                }
                else
                {
                    return result::ErrorExtensionNotSupported;
                }
            }

            return result::Success;
        }
    }
}
