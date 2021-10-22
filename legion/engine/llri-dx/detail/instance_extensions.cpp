/**
 * @file instance_extensions.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace llri
{
    namespace detail
    {
        bool queryInstanceExtensionSupport(instance_extension ext)
        {
            directx::lazyInitializeDirectX();

            switch (ext)
            {
                case instance_extension::DriverValidation:
                {
                    ID3D12Debug* temp = nullptr;
                    const bool succeeded = SUCCEEDED(directx::D3D12GetDebugInterface(IID_PPV_ARGS(&temp)));
                    if (succeeded)
                        temp->Release();
                    return succeeded;
                }
                case instance_extension::GPUValidation:
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
         * @brief Enables D3D12 validation layers.
         * @return If the needed debug interface is not found/supported, the function returns result::ErrorExtensionNotSupported.
        */
        result enableDriverValidationEXT()
        {
            ID3D12Debug* debugAPI = nullptr;
            if (SUCCEEDED(directx::D3D12GetDebugInterface(IID_PPV_ARGS(&debugAPI))))
            {
                debugAPI->EnableDebugLayer();
                debugAPI->Release();
                return result::Success;
            }

            return result::ErrorExtensionNotSupported;
        }

        /**
         * @brief Enables D3D12 GPU validation layers.
         * @return If the needed debug interface is not found/supported, the function returns result::ErrorExtensionNotSupported.
        */
        result enableGPUValidationEXT()
        {
            ID3D12Debug1* debugGPU = nullptr;
            if (SUCCEEDED(directx::D3D12GetDebugInterface(IID_PPV_ARGS(&debugGPU))))
            {
                debugGPU->SetEnableGPUBasedValidation(true);
                debugGPU->SetEnableSynchronizedCommandQueueValidation(true);
                debugGPU->Release();
                return result::Success;
            }

            return result::ErrorExtensionNotSupported;
        }
    }
}
