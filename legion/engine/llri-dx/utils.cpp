#include <llri/llri.hpp>
#include <graphics/directx/d3d12.h>

namespace legion::graphics::llri
{
    namespace internal
    {
        /**
         * @brief Function that maps an HRESULT to an llri::Result.
        */
        Result mapHRESULT(const HRESULT& value)
        {
            switch (HRESULT_CODE(value))
            {
                case S_OK :
                    return Result::Success;
                case DXGI_ERROR_WAIT_TIMEOUT :
                    return Result::Timeout;
                case DXGI_ERROR_DEVICE_HUNG :
                    return Result::ErrorDeviceHung;
                case DXGI_ERROR_DEVICE_REMOVED :
                    return Result::ErrorDeviceRemoved;
                case DXGI_ERROR_DEVICE_RESET :
                    return Result::ErrorDeviceLost;
                case DXGI_ERROR_DRIVER_INTERNAL_ERROR :
                    return Result::ErrorDriverFailure;
                case DXGI_ERROR_UNSUPPORTED :
                    return Result::ErrorFeatureNotSupported;
                case D3D12_ERROR_ADAPTER_NOT_FOUND:
                    break;
                case D3D12_ERROR_DRIVER_VERSION_MISMATCH:
                    return Result::ErrorIncompatibleDriver;
                case DXGI_ERROR_INVALID_CALL:
                    return Result::ErrorInvalidUsage;
                case DXGI_ERROR_WAS_STILL_DRAWING:
                    break;
                case E_FAIL:
                    break;
                case E_INVALIDARG:
                    return Result::ErrorInvalidUsage;
                case E_OUTOFMEMORY:
                    return Result::ErrorOutOfHostMemory;
                case E_NOTIMPL:
                    return Result::ErrorFeatureNotSupported;
                case S_FALSE:
                    return Result::NotReady;
            }

            return Result::ErrorUnknown;
        }
    }
}
