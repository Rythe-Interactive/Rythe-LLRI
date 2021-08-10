/**
 * @file instance_extensions.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace LLRI_NAMESPACE
{
    namespace detail
    {
        bool queryInstanceExtensionSupport(const instance_extension_type& type)
        {
            directx::lazyInitializeDirectX();

            switch (type)
            {
                case instance_extension_type::APIValidation:
                {
                    ID3D12Debug* temp = nullptr;
                    return SUCCEEDED(directx::D3D12GetDebugInterface(IID_PPV_ARGS(&temp)));
                }
                case instance_extension_type::GPUValidation:
                {
                    ID3D12Debug1* temp = nullptr;
                    return SUCCEEDED(directx::D3D12GetDebugInterface(IID_PPV_ARGS(&temp)));
                }
            }

            return false;
        }
    }

    namespace internal
    {
        /**
         * @brief Enables D3D12 API validation layers where requested.
         * @return If the needed debug interface is not found/supported, the function returns result::ErrorExtensionNotSupported.
        */
        result createAPIValidationEXT(const api_validation_ext& ext, void** output)
        {
            if (ext.enable)
            {
                ID3D12Debug* debugAPI = nullptr;
                if (SUCCEEDED(directx::D3D12GetDebugInterface(IID_PPV_ARGS(&debugAPI))))
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
        result createGPUValidationEXT(const gpu_validation_ext& ext, void** output)
        {
            if (ext.enable)
            {
                ID3D12Debug1* debugGPU = nullptr;
                if (SUCCEEDED(directx::D3D12GetDebugInterface(IID_PPV_ARGS(&debugGPU))))
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
