/**
 * @file utils.cpp
 * @copyright 2021-2021 Leon Brands. All rights served.
 * @license: https://github.com/Legion-Engine/Legion-LLRI/blob/main/LICENSE
 */

#include <llri/llri.hpp>
#include <llri-dx/directx.hpp>

namespace LLRI_NAMESPACE
{
    namespace internal
    {
        /**
         * @brief Function that maps an HRESULT to an llri::result.
        */
        result mapHRESULT(const HRESULT& value)
        {
            switch (HRESULT_CODE(value))
            {
                case S_OK :
                    return result::Success;
                case DXGI_ERROR_WAIT_TIMEOUT :
                    return result::Timeout;
                case DXGI_ERROR_DEVICE_HUNG :
                    return result::ErrorDeviceHung;
                case DXGI_ERROR_DEVICE_REMOVED :
                    return result::ErrorDeviceRemoved;
                case DXGI_ERROR_DEVICE_RESET :
                    return result::ErrorDeviceLost;
                case DXGI_ERROR_DRIVER_INTERNAL_ERROR :
                    return result::ErrorDriverFailure;
                case DXGI_ERROR_UNSUPPORTED :
                    return result::ErrorFeatureNotSupported;
                case D3D12_ERROR_ADAPTER_NOT_FOUND:
                    break;
                case D3D12_ERROR_DRIVER_VERSION_MISMATCH:
                    return result::ErrorIncompatibleDriver;
                case DXGI_ERROR_INVALID_CALL:
                    return result::ErrorInvalidUsage;
                case DXGI_ERROR_WAS_STILL_DRAWING:
                    break;
                case E_FAIL:
                    break;
                case E_NOINTERFACE:
                    return result::ErrorExtensionNotSupported;
                case E_INVALIDARG:
                    return result::ErrorInvalidUsage;
                case E_OUTOFMEMORY:
                    return result::ErrorOutOfHostMemory;
                case E_NOTIMPL:
                    return result::ErrorFeatureNotSupported;
                case S_FALSE:
                    return result::NotReady;
            }

            return result::ErrorUnknown;
        }
    }
}
