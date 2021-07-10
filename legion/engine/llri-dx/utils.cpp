#include <llri/llri.hpp>
#include <graphics/directx/d3d12.h>

namespace legion::graphics::llri
{
    namespace internal
    {
        /**
         * @brief Constexpr function that maps a HRESULT to an llri::Result.
        */
        Result mapHRESULT(const HRESULT& value)
        {
            switch (HRESULT_CODE(value))
            {
                case static_cast<long>(0) :
                    return Result::Success;
                case static_cast<long>(0x887A0027) :
                    return Result::Timeout;
                case static_cast<long>(0x887A0006) :
                    return Result::ErrorDeviceHung;
                case static_cast<long>(0x887A0005) :
                    return Result::ErrorDeviceRemoved;
                case static_cast<long>(0x887A0007) :
                    return Result::ErrorDeviceLost;
                case static_cast<long>(0x887A0020) :
                    return Result::ErrorDriverFailure;
                case static_cast<long>(0x887A0004) :
                    return Result::ErrorFeatureNotSupported;
                default:
                    return Result::ErrorUnknown;
            }
        }
    }
}
