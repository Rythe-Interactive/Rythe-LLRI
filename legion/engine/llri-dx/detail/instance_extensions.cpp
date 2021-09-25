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
        bool queryInstanceExtensionSupport(instance_extension_type type)
        {
            directx::lazyInitializeDirectX();

            switch (type)
            {
                case instance_extension_type::DriverValidation:
                {
                    ID3D12Debug* temp = nullptr;
                    const bool succeeded = SUCCEEDED(directx::D3D12GetDebugInterface(IID_PPV_ARGS(&temp)));
                    if (succeeded)
                        temp->Release();
                    return succeeded;
                }
                case instance_extension_type::GPUValidation:
                {
                    ID3D12Debug1* temp = nullptr;
                    const bool succeeded = SUCCEEDED(directx::D3D12GetDebugInterface(IID_PPV_ARGS(&temp)));
                    if (succeeded)
                        temp->Release();
                    return succeeded;
                }
            }

            return false;
        }
    }

    namespace internal
    {
        /**
         * @brief Enables D3D12 validation layers where requested.
         * @return If the needed debug interface is not found/supported, the function returns result::ErrorExtensionNotSupported.
        */
        result createDriverValidationEXT(const driver_validation_ext& ext, void** output)
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
